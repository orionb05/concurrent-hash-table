#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#include "rwlock.h"

typedef enum {
    OP_INSERT,
    OP_UPDATE,
    OP_DELETE,
    OP_SEARCH,
    OP_PRINT
} OperationType;

typedef struct {
    OperationType op;
    char name[50];
    uint32_t salary;
    int priority;
} CommandInfo;

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

typedef struct
{
  hashRecord *head;
  rwlock_t lock;
} HashTable;

#endif