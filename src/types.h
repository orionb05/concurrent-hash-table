#ifndef TYPES_H
#define TYPES_H

/*
// Contains all types (except rwlock) used throughout the program. 
*/

#include <stdint.h>

#include "rwlock.h"

// Used by logger to determine the structure of it's ouput for each command
typedef enum {
    OP_INSERT,
    OP_UPDATE,
    OP_DELETE,
    OP_SEARCH,
    OP_PRINT
} OperationType;

// Base node for the hashtable
typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

/*
// Passed into functions to track progression of command completion and info.
// To be updated by argument parser and hashtable logic.
*/
typedef struct {
    OperationType op;
    char name[50];
    uint32_t salary;
    int priority;
    int succeeded;
    hashRecord *record;
} CommandInfo;

// Core table components, including head and lock. Should be instantiated once.
typedef struct
{
  hashRecord *head;
  rwlock_t lock;
} HashTable;

#endif