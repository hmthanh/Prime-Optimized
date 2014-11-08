#include <math.h>
#include <stdlib.h>

#include "prime32.h"

/*
Copyright (c) 2004 Paul Hsieh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    Neither the name of prime32 nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/* Knuth's modified gcd algorithm.  Rather than testing for divisibility by
   small primes one at a time, one can simply test the gcd of the number with
   a product such as 3*5*7*11*13*17*19*23. */

#if defined (__WATCOMC__)
unsigned long gcd (unsigned long a, unsigned long b);
#pragma aux gcd    = \
"  .686            " \
"  mov   ecx, ebx  " \
"  or    ebx, edx  " \
"  mov   eax, 1    " \
"  jz    L2        " \
"  test  ecx, ecx  " \
"  mov   eax, edx  " \
"  je    L2        " \
"  test  edx, edx  " \
"  mov   eax, ecx  " \
"  je    L2        " \
"  bsf   ebx, eax  " \
"  bsf   ecx, edx  " \
"  cmp   ebx, ecx  " \
"  mov   edi, ecx  " \
"  cmovl edi, ebx  " \
"  shr   edx, cl   " \
"  mov   ecx, ebx  " \
"  shr   eax, cl   " \
"  jmp   L0        " \
"L3:               " \
"  cmovb eax, edx  " \
"  cmovb edx, ecx  " \
"  sub   eax, edx  " \
"  bsf   ecx, eax  " \
"  shr   eax, cl   " \
"L0:               " \
"  cmp   eax, edx  " \
"  mov   ecx, eax  " \
"  jne   L3        " \
"L1:               " \
"  mov   ecx, edi  " \
"  shl   eax, cl   " \
"L2:               " \
parm [edx] [ebx] modify [ecx edi] value [eax];
#elif defined(_MSC_VER)
static uint32_t gcd (uint32_t a, uint32_t b) {
int retv = 0;
    _asm {
       mov   ebx, dword ptr [b]
       mov   edx, dword ptr [a]
       mov   ecx, ebx
       or    ebx, edx
       mov   eax, 1
       jz    L2
       test  ecx, ecx
       mov   eax, edx
       je    L2
       test  edx, edx
       mov   eax, ecx
       je    L2
       bsf   ebx, eax
       bsf   ecx, edx
       cmp   ebx, ecx
       mov   edi, ecx
       cmovl edi, ebx
       shr   edx, cl
       mov   ecx, ebx
       shr   eax, cl
       jmp   L0
     L3:
       cmovb eax, edx
       cmovb edx, ecx
       sub   eax, edx
       bsf   ecx, eax
       shr   eax, cl
     L0:
       cmp   eax, edx
       mov   ecx, eax
       jne   L3
     L1:
       mov   ecx, edi
       shl   eax, cl
     L2:
       mov   dword ptr [retv], eax
    }
    return retv;
}
#else

/* Euclid's modified gcd algorithm.  There is a bug in my Knuth's code for
   > 31 bits, so I am just going back to the standard algorithm (which is
   not slower on average versus Knuth's algorithm anyway.) */

static uint32_t gcd (uint32_t a, uint32_t b) {
uint32_t c;

    if (a < b) { 
        if (a == 0) return b;
        c = a; a = b; b = c;
    }

    if (a == 0) return 1;

    while (b > 1) {
        if (0 == (a %= b)) return b;
        c = a; a = b; b = c;
    }

    if (b == 1) return 1;
    return a;
}
#endif

/* Deal with the primes up to about a 512000.  Implements a standard
   sieve with 1 bit per entry, skipping even numbers.  Obviously one can
   dramatically speed this up by increasing QF_SIEVE_SIZE, however, this
   will chew up L1 D-cache in your CPU and increase the data footprint
   which may not be acceptable in realworld applications. */

#define QF_SIEVE_SIZE (8*1024)
static uint32_t qfSieve[QF_SIEVE_SIZE];
#define qfsIdx(x) ((x) >> 6)
#define qfsBit(x) (1 << (((x) & 63) >> 1))

static void initQFSieve (void) {
int i, j, p;
    for (i=0; i < QF_SIEVE_SIZE; i++) qfSieve[i] = 0;
    qfSieve[0] = 1;
    for (p=3; p < (QF_SIEVE_SIZE * 64); p += 2) {
        if ((qfSieve[qfsIdx (p)] & qfsBit (p)) != 0) continue;
        j = p + p;
        for (i = j + p; i < (QF_SIEVE_SIZE * 64); i += j) {
            qfSieve[qfsIdx (i)] |= qfsBit (i);
        }
    }
}

#ifndef QPT_COMP
#define QPT_COMP  0
#endif
#ifndef QPT_PRIME
#define QPT_PRIME 1
#endif
#ifndef QPT_DUNNO
#define QPT_DUNNO 2
#endif

/* Test for trivial primeness, otherwise use the table precalculated by the
   sieve of eratosthenes */

static uint32_t quickPrimeTest (uint32_t l) {

    /* Weed out 2 and 3 */

    if (l - 2 < 2) return QPT_PRIME;

    /* Knock out even numbers above 2 */

    if ((l & 1) == 0 || l == 1) return QPT_COMP;

    if (l >= (QF_SIEVE_SIZE * 64)) {
        /* Small prime test, wipes out 62% of remaining composite
           numbers, making for a total of 81% weed out. */

        if (gcd ((3*5*7*11*13*17*19*23), l) > 1) return QPT_COMP;
        return QPT_DUNNO;
    }

    /* Sieve of eratosthenes */

    if ((qfSieve[qfsIdx (l)] & qfsBit (l)) == 0) return QPT_PRIME;
    return QPT_COMP;
}

/* Return the residue of (x * y) mod m */

#if defined (__WATCOMC__)
uint32_t mulMod (uint32_t x, uint32_t y, uint32_t m);
#pragma aux mulMod = \
"   mul edx " \
"   div ebx " \
parm [eax] [edx] [ebx] modify [eax edx] value [edx];
#else
static uint32_t mulMod (uint32_t x, uint32_t y, uint32_t m) {
#if defined(_MSC_VER)
    x = x;
    m = m;
    _asm {
        mov eax, x
        mov edx, y
        mov ebx, m
        mul edx
        div ebx
        mov y, edx
    }
    return y;
#else
unsigned uint64_t a;
uint32_t b;

    a = ((uint64_t) x) * ((uint64_t) y);
    b = (uint32_t) (a / m);
    return ((uint32_t) a) - (b * m);
#endif
}
#endif

/* Use the standard successive squaring trick, to compute raising to an
   integer power */

static uint32_t powMod (uint32_t x, uint32_t y, uint32_t m) {
uint32_t j;

    if (x > m) x %= m;

    if (x == 1 || y == 0) return 1;
    if (x == 0) return 0;
    if (x == -1) {
        if (y & 1) return m - 1;
        return 1;
    }

    j = 1;
    while (y) {
        if (y & 1) j = mulMod (j, x, m);
        y >>= 1;
        x = mulMod (x, x, m);
    }

    return j;
}

/* Strong pseudo-prime test.  ref: Bressoud, David M. "Factorization and
   Primality Testing" */

static int unsigned SPPTestBase (uint32_t n, uint32_t b) {
uint32_t r1, r2;
int a;

    r1 = n-1;
    r2 = n-1;

    a = 0;
    while ((r1 & 1) == 0) {
        r1 >>= 1;
        a++;
    }

    r1 = powMod (b, r1, n);

    if (r1 == 1 || r1 == r2) return 1;

    while (--a) {
        r1 = mulMod (r1, r1, n);
        if (r1 == r2) return 1;
    }

    return 0;
}

/* Known to be correct up to 10^12 or so.  ref: Jaeschke, Gerhard. "On strong
   pseudoprimes to several bases" */

static int unsigned SPPTest (uint32_t n) {
    return  SPPTestBase (n, 2) &&
            SPPTestBase (n, 13) &&
            SPPTestBase (n, 23) &&
            SPPTestBase (n, 1662803);
}

/* Special prime test for factoring that assumes that trial division with 
   small primes has already been performed. */

static int isPrimeNoTrialDivision (uint32_t n) {
    if (n < (QF_SIEVE_SIZE * 64)) {
        if ((qfSieve[qfsIdx (n)] & qfsBit (n)) == 0) return QPT_PRIME;
        return QPT_COMP;
    }

    if (n == 1662803 || SPPTest (n)) return QPT_PRIME;
    return QPT_COMP;
}

/* Combine the quick test with the strong pseudoprime test */

int isPrime (uint32_t n) {
int r;
    if ((r = quickPrimeTest (n)) != QPT_DUNNO) return r;
    if (n == 1662803 || SPPTest (n)) return QPT_PRIME;
    return QPT_COMP;
}

/*
   Brent's modification of the Pollard algorithm

   This algorithm appears to be vastly slower than the strong pseudo prime 
   test, but of course its faster than the brute force division test.
*/

static int pollardRhoFactor (uint32_t * res, uint32_t f) {
uint32_t i, y;
static uint32_t x0, x1, x2;

    x0 = 2;
    x1 = 2;

    for (i=0; i < 1024; i++) {

        /* Use the primitive polynomial x' = x^2-x+1 */
        x2 = mulMod (x1, x1 - 1, f) + 1;

        if (x2 > x0) y = x2 - x0;
        else y = x0 - x2;

        x1 = x2;
        y = gcd (y, f);

        if (y > 1 && y != f) {
            *res = y;
            return 1;
        }

        /* If power of 2, drop plumb bob */
        if (0 == (i & (i - 1))) x0 = x2;
    }
    return 0;
}

static uint32_t firstPrimes[] = {
    2,    3,    5,    7,   11,   13,   17,   19,   23,   29,   31,   37,
   41,   43,   47,   53,   59,   61,   67,   71,   73,   79,   83,   89,
   97,  101,  103,  107,  109,  113,  127,  131,  137,  139,  149,  151,
  157,  163,  167,  173,  179,  181,  191,  193,  197,  199,  211,  223,
  227,  229,  233,  239,  241,  251,  257,  263,  269,  271,  277,  281,
  283,  293,  307,  311,  313,  317,  331,  337,  347,  349,  353,  359,
  367,  373,  379,  383,  389,  397,  401,  409,  419,  421,  431,  433,
  439,  443,  449,  457,  461,  463,  467,  479,  487,  491,  499,  503,
  509,  521,  523,  541  };

#define NUM_FIRST_PRIMES (sizeof (firstPrimes)/sizeof (uint32_t))

/* Fast factoring, first by quickly scanning against the first 100 primes
   then checking for primality then by trying the pollard rho algorithm then
   doing an exhaustive search. */

uint32_t getFactor (uint32_t l) {
uint32_t f, d, tt;

    if (l < 4) return l;
    if ((l&1) == 0) return 2;                                    /*     50% */

    /* Trial division of small primes */
    for (tt=1; tt < NUM_FIRST_PRIMES; tt++) {
        if ((l % (d = firstPrimes[tt])) == 0) return d;          /*     91% */
    }

    /* If its a prime, don't waste further time testing it */
    if (isPrimeNoTrialDivision (l) == QPT_PRIME) return l;       /*     96% */

    /* Try factoring it using pollardRho */
    if (pollardRhoFactor (&f, l)) return f;                      /* 99.997% */

    /* Simply brute force test against all odd numbers above 541.  We know
       that this function is supposed to terminate because we know the
       number is not a prime.  This is an example of where the halting
       problem is not trivial to decide. */
    f = firstPrimes[NUM_FIRST_PRIMES - 1];
    do {
        f += 2;
        if (0 == (l % f)) return f;                              /*    100% */
    } while (1);
}

static int primesStarted = 0;

int primeInit (void) {
    if (primesStarted) return -__LINE__;
    initQFSieve ();
    primesStarted = 1;
    return 0;
}
