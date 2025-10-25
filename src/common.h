// Adapted from OSTEP rwlock.c by Remzi Arpaci-Dusseau
// Source: https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c

#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void Spin(int howlong) {
    double t = GetTime();
    while ((GetTime() - t) < (double) howlong)
	; // do nothing in loop
}
