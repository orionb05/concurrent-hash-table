#ifndef RWLOCK_H
#define RWLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <semaphore.h>

#include "common.h"
#include "common_threads.h"

// Adapted from OSTEP rwlock.c by Remzi Arpaci-Dusseau
// Source: https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c

typedef struct _rwlock_t {
    sem_t writelock;
    sem_t lock;
    int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock);

void rwlock_acquire_readlock(rwlock_t *lock);

void rwlock_release_readlock(rwlock_t *lock);

void rwlock_acquire_writelock(rwlock_t *lock);

void rwlock_release_writelock(rwlock_t *lock);

#endif