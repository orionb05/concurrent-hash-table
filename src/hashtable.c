#include "hashtable.h"
#include "common.h"
#include "types.h"
#include "logging.h"
#include <stdbool.h>
#include <stdlib.h>

void insert(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    uint32_t salary = command->salary;
    int priority = command->priority;

    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the insert operation
    char msg[64];
    snprintf(msg, sizeof(msg), "INSERT,%u,%s,%u", hash, name, salary);
    PrintLog(GetMicroTime(), priority, msg);

    // Set up rwlock and log
    ts_wait = GetMicroTime();
    rwlock_acquire_writelock(lock);
    ts_aqr = GetMicroTime();
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    // Check for duplicate
    hashRecord *curr = table->head;
    while(curr != NULL) {
        if(curr->hash == hash) {
            // Duplicate found
            command->succeeded = false;
            command->record = curr;
            PrintUpdate(command);
            
            ts_rel = GetMicroTime();
            rwlock_release_writelock(lock);
            PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");
            return;
        }
        curr = curr->next;
    }

    // Create new record
    hashRecord *newRecord = (hashRecord*)malloc(sizeof(hashRecord));
    if (newRecord == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(1);
    }
    newRecord->hash = hash;
    strcpy(newRecord->name, name);
    newRecord->salary = salary;
    newRecord->next = table->head;
    table->head = newRecord;

    command->succeeded = true;
    command->record = newRecord;
    PrintUpdate(command);
    
    ts_rel = GetMicroTime();
    rwlock_release_writelock(lock);
    PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");
}

int delete(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    int priority = command->priority;
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the delete operation
    char msg[64];
    snprintf(msg, sizeof(msg), "DELETE,%u,%s", hash, name);
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
            // Record found - save info before deleting
            command->succeeded = true;
            command->record = curr;
            command->salary = curr->salary;
            
            // Delete it
            if (prev == NULL) { // Deleting the first node
                table->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            
            PrintUpdate(command);
            free(curr);
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

int updateSalary(HashTable *table, CommandInfo *command) {
    const char *name = command->name;
    uint32_t newSalary = command->salary;
    int priority = command->priority;
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Log the update operation
    char msg[64];
    snprintf(msg, sizeof(msg), "UPDATE,%u,%s,%u", hash, name, newSalary);
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
            command->oldSalary = oldSalary;  // Save old salary for printing
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
    char msg[64];
    snprintf(msg, sizeof(msg), "PRINT");
    PrintResults(table);
}

hashRecord* search(HashTable *table, CommandInfo *command){
    
    const char *name = command->name;
    int priority = command->priority;

    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    long long ts_wait, ts_aqr, ts_rel;

    // Construct and save the primary log message
    char msg[64];
    snprintf(msg, sizeof(msg), "SEARCH,%u,%s", hash, name);
    PrintLog(GetMicroTime(), priority, msg);

    // Set up rwlock and log
    ts_wait = GetMicroTime();
    rwlock_acquire_readlock(lock);
    ts_aqr = GetMicroTime();
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "READ LOCK ACQUIRED");

    // Set up for traversal over hash table
    hashRecord *curr = table->head;

    while(curr != NULL){
        if(curr->hash == hash) break;
        curr = curr->next;
    }

    if(curr != NULL){
        command->succeeded = true;
        command->record = curr;
        command->salary = curr->salary;
    } else {
        command->succeeded = false;
    }

    PrintUpdate(command);

    // Log and release lock
    ts_rel = GetMicroTime();
    rwlock_release_readlock(lock);
    PrintLog(ts_rel, priority, "READ LOCK RELEASED");

    return curr;
}

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