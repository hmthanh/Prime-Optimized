#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prime32.h"
#include "primeat.h"

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

#define NVALS (1024*1024)

int factorTest (void) {
long i, x;
    srand (0); /*time (NULL)); */
    for (x=0,i=0; i < NVALS; i++) {
        uint32_t r = ((rand () << 16) + rand ()); /* & 0x7FFFFFFF; */
        uint32_t f = getFactor (r);
        x += f;
    }
    return x;
}

int main (int argc, char * argv[]) {
unsigned int i;
clock_t start, end;

    start = clock ();

    /* Must be called to initialize the sieve of eratosthenes */
    primeInit ();

    for (i=10; i < 1000000000; i *= 10) {
        printf ("The %uth prime is %u\n", i, primeAt (i));
    }

    end = clock ();
    printf ("%.3fs\n", (end - start)/((double) CLOCKS_PER_SEC));

    start = clock ();
    factorTest();
    end = clock ();
    printf ("factorings per second: %.0f\n", (((double) CLOCKS_PER_SEC)*NVALS) / (end - start));

    return 0;
}

