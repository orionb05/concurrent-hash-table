#include "hashtable.h"
#include "common.h"
#include "types.h"
#include "logging.h"
#include <stdbool.h>
#include <stdlib.h>

/*
// Takes a command and creates a new record if the name isn't
// already within the table. Otherwise, it updates the record
// with the new name.
*/
int cmp(const void *a, const void *b) {
        hashRecord *ha = *(hashRecord**)a;
        hashRecord *hb = *(hashRecord**)b;
        return (ha->hash > hb->hash) - (ha->hash < hb->hash);
    }
void insert(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    uint32_t salary = command->salary;
    int priority = command->priority;

    int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the insert operation
    char msg[64];
    snprintf(msg, sizeof(msg), "INSERT,%d,%s,%d", hash, name, salary);
    PrintLog(GetMicroTime(), priority, msg);

    // Set up rwlock and log
    ts_wait= GetMicroTime();
    rwlock_acquire_writelock(lock);
    ts_aqr = GetMicroTime();
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    // Insertion logic...
    // (similar to the previous insert logic)
    
    ts_rel = GetMicroTime();
    rwlock_release_writelock(lock);
    PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");
}

// Deletes the record with the specified name from the hash table.
int delete(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    int priority = command->priority;
    int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the delete operation
    char msg[64];
    snprintf(msg, sizeof(msg), "DELETE,%d,%s", hash, name);
    PrintLog(GetMicroTime(), priority, msg);

    // Acquire write lock
    ts_wait = GetMicroTime();
    rwlock_acquire_writelock(lock);
    ts_aqr = GetMicroTime();
    if ((ts_aqr - ts_wait) > 10) {
        PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    }
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    hashRecord *curr = table->head;
    hashRecord *prev = NULL;

    // Traverse the hash table to find the record
    while (curr != NULL) {
        if (curr->hash == hash) {
            // Record found, delete it
            if (prev == NULL) { // Deleting the first node
                table->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            command->succeeded = true;
            PrintUpdate(command);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    // If no record found
    if (curr == NULL) {
        command->succeeded = false;
        PrintUpdate(command);
    }

    // Release write lock
    ts_rel = GetMicroTime();
    rwlock_release_writelock(lock);
    PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");

    return 0;
}

// Updates the salary of the record with the specified name in the hash table.
int updateSalary(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    uint32_t newSalary = command->salary;
    int priority = command->priority;
    int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the update operation
    char msg[64];
    snprintf(msg, sizeof(msg), "UPDATE,%d,%s,%d", hash, name, newSalary);
    PrintLog(GetMicroTime(), priority, msg);

    // Acquire write lock
    ts_wait = GetMicroTime();
    rwlock_acquire_writelock(lock);
    ts_aqr = GetMicroTime();
    if ((ts_aqr - ts_wait) > 10) {
        PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    }
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    hashRecord *curr = table->head;

    // Traverse the hash table to find the record
    while (curr != NULL) {
        if (curr->hash == hash) {
            // Record found, update salary
            uint32_t oldSalary = curr->salary;
            curr->salary = newSalary;
            command->succeeded = true;
            command->record = curr;
            command->salary = newSalary;

            // Log the updated record
            PrintUpdate(command);
            break;
        }
        curr = curr->next;
    }

    // If no record found
    if (curr == NULL) {
        command->succeeded = false;
        PrintUpdate(command);
    }

    // Release write lock
    ts_rel = GetMicroTime();
    rwlock_release_writelock(lock);
    PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");

    return 0;
}

void printTable(HashTable *table, CommandInfo *command) {
    int priority = command->priority;
    rwlock_t *lock = &table->lock;

    long long ts_wait = GetMicroTime();
    rwlock_acquire_readlock(lock);
    long long ts_aqr = GetMicroTime();

    if ((ts_aqr - ts_wait) > 10)
        PrintLog(ts_wait, priority, "WAITING FOR MY TURN");

    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "READ LOCK ACQUIRED");

    // Count nodes
    int count = 0;
    hashRecord *curr = table->head;
    while (curr) {
        count++;
        curr = curr->next;
    }

    // Collect pointers
    hashRecord **arr = malloc(sizeof(hashRecord*) * count);
    curr = table->head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }

    qsort(arr, count, sizeof(hashRecord*), cmp);

    printf("Current Database:\n");
    for (int i = 0; i < count; i++) {
        printf("%u,%s,%u\n", arr[i]->hash, arr[i]->name, arr[i]->salary);
    }

    free(arr);

    long long ts_rel = GetMicroTime();
    rwlock_release_readlock(lock);
    PrintLog(ts_rel, priority, "READ LOCK RELEASED");
}

/*
// Searches the table for the name within the command. Returns the record
// if the name is found. Return NULL otherwise.
*/
hashRecord* search(HashTable *table, CommandInfo *command){
    
    const char *name = command->name;
    int priority = command->priority;

    int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    // Track times around lock interactions
    long long ts_wait, ts_aqr, ts_rel;

    // Construct and save the primary log message
    char msg[64];
    snprintf(msg, sizeof(msg), "SEARCH,%d,%s", hash, name);
    PrintLog(GetMicroTime(), priority, msg);

    // Set up rwlock and log
    ts_wait= GetMicroTime();
    rwlock_acquire_readlock(lock);
    ts_aqr = GetMicroTime();
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    // Simulate waiting & awakening
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "READ LOCK ACQUIRED");

    // Set up for traversal over hash table
    hashRecord *curr = table->head;

    while(curr != NULL){ // Navigate to either the query node or the end of the list

        if(curr->hash == hash) break;

        curr = curr->next;
    }

    if(curr != NULL){ // Save info in command for remaining logging

        command->succeeded = true;
        command->record = curr;
        command->salary = curr->salary;
    }

    PrintUpdate(command);

    // Log and release lock
    ts_rel = GetMicroTime();
    rwlock_release_readlock(lock);
    PrintLog(ts_rel, priority, "READ LOCK RELEASED");

    return curr;
}

// Cleans malloc'd space
int freeTable(HashTable *table){
    
    hashRecord *curr = table->head;
    hashRecord *next = NULL;

    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }

    return 1;
}