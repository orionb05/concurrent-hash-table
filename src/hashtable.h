#ifndef HASHTABLE_H
#define HASHTABLE_H

// Concurrent hash table interface for key–salary mapping

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

int insert(HashTable *table, CommandInfo *command);

int delete(char name[]);

int updateSalary(char name[], uint32_t salary);

int search(HashTable *table, CommandInfo *command);

int freeTable(HashTable *table);

#endif