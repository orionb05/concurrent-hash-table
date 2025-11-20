#ifndef HASHTABLE_H
#define HASHTABLE_H

// Primary concurrent hash table interface for key–salary mapping

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

void insert(HashTable *table, CommandInfo *command);

int delete(HashTable *table, CommandInfo *command);

void printTable(HashTable *table, CommandInfo *command);

int updateSalary(HashTable *table, CommandInfo *command);

hashRecord* search(HashTable *table, CommandInfo *command);

int freeTable(HashTable *table);

#endif