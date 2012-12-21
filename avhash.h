/* Average hashing implemented in C */
#ifndef AV_HASH_H
#define AV_HASH_H

struct bitvec;
struct bitvec* bitvec_new(int w, int h);
void bitvec_free(struct bitvec **bv);

struct bitvec* avhash(const unsigned char *data, int w, int ws, int h);
int hamming(const struct bitvec*, const struct bitvec* );

#endif
