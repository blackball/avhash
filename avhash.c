#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

typedef unsigned int uint;
typedef unsigned char uchar;

#define BITS_PER_UINT (sizeof(uint) * CHAR_BIT)

struct bitvec {
    uint len;
    uint code[1];
};

struct bitvec*
bitvec_new(int w, int h) {
    /* create bitvec by image width and height */
    struct bitvec *bv = 0;
    int len = (w * h) / BITS_PER_UINT - 1;
    if (len < 0) len = 0;
    bv = (struct bitvec*)calloc(sizeof(*bv) + sizeof(uint) * len, 1);
    bv->len = len;
    return bv;
}

void
bitvec_free(struct bitvec **bv) {
    if (bv) {
	free( *bv );
	*bv = 0;
    }
}

/* turn the bit to 1 in position pos */
#define bitvec_set(bv, pos)						\
    (bv->code[pos / BITS_PER_UINT] |= (1U << (pos % BITS_PER_UINT)))

static inline uint
bit_count(uint i) {
    /* count 1 in uint */
    uint n = 0;
    while (i) {
        n++;
        i &= (i-1);
    }
    return n;
}

uint
hamming(const struct bitvec *bv0, const struct bitvec *bv1) {
    /* calculate the hamming distance of two bitvec */
    uint dist = 0;

    for (uint i = 0; i < bv0->len; ++i) {
        dist += bit_count(bv0->code[i] ^ bv1->code[i]);
    }

    return dist;
}

struct bitvec*
avhash(const uchar *data, int w, int ws, int h) {
    /* average hashing */
    int counter = 0, avg = 0;
    double davg = .0;

    struct bitvec *bv = bitvec_new(w, h);
    
    for (int i = 0; i < h; ++i)
	for (int j = 0; j < w; ++j) 
	    davg += data[i*ws + j];

    avg = davg / (w * h);
    
    for (int i = 0; i < h; ++i)
	for (int j = 0; j < w; ++j) {
	    if (data[i*ws + j] > avg) {
		bitvec_set(bv, counter);
	    }
	    ++ counter;
	}
    
    return bv;
}


static void 
print_bin(const struct bitvec *bv) {
    /* print binary representation */
    for (uint i = 0; i < bv->len; ++i) {
	uint t = bv->code[i];
	for (uint j = 1U << (BITS_PER_UINT - 1); j != 0; j >>= 1) {
	    (t&j) ? putchar('0') : putchar('1');
	}
    }
    putchar('\n');
}

/* a simple unit test */
int
main(int argc, char *argv[]) {
    const char *name0 = "t.jpg", *name1 = "y.jpg";
    const CvSize sz = cvSize(16, 32);
    struct bitvec *bv0, *bv1;
    IplImage *img, *gray;
    uint hamming_dist = 0;

    if (argc == 3) {
	name0 = argv[1];
	name1 = argv[2];
    }
    
    gray = cvCreateImage(sz, 8, 1);

    img = cvLoadImage(name0, 0);
    cvResize(img, gray, CV_INTER_LINEAR);

    bv0 = avhash((const uchar *)(gray->imageData), gray->width, gray->widthStep, gray->height);

    cvReleaseImage(&img);

    img = cvLoadImage(name1, 0);
    cvResize(img, gray, CV_INTER_LINEAR);

    bv1 = avhash((const uchar *)(gray->imageData), gray->width, gray->widthStep, gray->height);

    cvReleaseImage(&img);
    cvReleaseImage(&gray);
    
    print_bin(bv0);
    print_bin(bv1);
    hamming_dist = hamming(bv0, bv1);

    bitvec_free(&bv0);
    bitvec_free(&bv1);

    return 0;
}
