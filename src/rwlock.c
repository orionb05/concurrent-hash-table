#include "rwlock.h"

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    Sem_init(&lock->lock, 1); 
    Sem_init(&lock->writelock, 1); 
}

long long rwlock_acquire_readlock(rwlock_t *lock) {
    Sem_wait(&lock->lock);
    lock->readers++;
    if (lock->readers == 1)
	Sem_wait(&lock->writelock);
    Sem_post(&lock->lock);
    return GetMicroTime();
}

long long rwlock_release_readlock(rwlock_t *lock) {
    Sem_wait(&lock->lock);
    lock->readers--;
    if (lock->readers == 0)
	Sem_post(&lock->writelock);
    Sem_post(&lock->lock);
    return GetMicroTime();
}

long long rwlock_acquire_writelock(rwlock_t *lock) {
    Sem_wait(&lock->writelock);
    return GetMicroTime();
}

long long rwlock_release_writelock(rwlock_t *lock) {
    Sem_post(&lock->writelock);
    return GetMicroTime();
}
    

