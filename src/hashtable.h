#ifndef HASHTABLE_H
#define HASHTABLE_H

// Primary concurrent hash table interface for key–salary mapping

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

void insert(HashTable *table, CommandInfo *command);

int delete(char name[]);

int updateSalary(char name[], uint32_t salary);

hashRecord* search(HashTable *table, CommandInfo *command);

int freeTable(HashTable *table);

#endif