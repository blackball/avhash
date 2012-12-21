#include "avhash.h"
#include <limits.h>

typedef unsigned int uint;

#ifdef _DEBUG
#define xassert(expr) assert(expr)
#else
#define xassert(expr) 
#endif

#define BITS_PER_UINT (sizeof(uint) * CHAR_BITS))

struct bitvec {
    uint len;
    uint code[1];
};

struct bitvec*
bitvec_new(int w, int h) {
    /* create bitvec by image width and height */
    int len = (w * h) / BITS_PER_UINT - 1;
    if (len < 0) len = 0;
    return (struct bitvec*)calloc(sizeof(*bv) + sizeof(uint) * len, 1);
}

void
bitvec_free(struct bitvec **bv) {
    xassert( *bv );
    free( *bv );
    *bv = 0;
}

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

inline int
hamming(const struct bitvec *bv0, const struct bitvec *bv1) {
    /* calculate the hamming distance of two bitvec */
    uint dist = 0;

    xasssert( bv0 && bv1 && (bv0->len == bv1->len) );

    for (int i = 0; i < bv0->len; ++i) {
        dist += bit_count(bv0->code[i] ^ bv1->code[i]);
    }

    return dist;
}

struct bitvec*
avhash(const unsigned char *data, int w, int ws, int h) {
    int counter = 0;
    int avg = 0;
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
