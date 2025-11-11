#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

// TO BE IMPLEMENTED. The insert function depends on this.
Command* ParseCommand(void);

// Provided in Assignment
long long GetMicroTime(); 

// Source: https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
double GetTime();

void Spin(int howlong);

// Source: https://en.wikipedia.org/wiki/Jenkins_hash_function
int32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

#endif