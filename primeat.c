#include "primeat.h"
static
#include "primeidx.h"

static int estPrimeIdx (int * c, uint32_t * p, long idx) {
    if (idx <= 0) return -__LINE__;
    idx &= ~(32767l);
    if (idx == 0) {
        *c = 1;
        *p = 2;
        return 0;
    }
    *c = idx;
    idx = (idx >> 15) - 1;
    if (idx >= nPrimeIdxs) return -__LINE__;
    *p = primeIdx[idx];
    return 0;
}

/* For index -> prime algorithm perform a table look up to find the closest
   (index -> prime) pair lower than the one requested, then simply scan ahead
   until the precisely indexed prime is found. */

uint32_t primeAt (long i) {
uint32_t p;
int c, r = estPrimeIdx (&c, &p, i);
    if (r < 0) return 0;
    for (;p != 0;) {
        if (isPrime (p) == QPT_PRIME) {
            if (c == i) break;
            c++;
        }
        p++;
    }
    return p;
}

