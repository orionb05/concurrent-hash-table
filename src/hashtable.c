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
void insert(HashTable *table, CommandInfo *command){
    
    const char *name = command->name;
    uint32_t salary = command->salary;
    int priority = command->priority;

    int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    rwlock_t *lock = &table->lock;
    // Track times around lock interactions
    long long ts_wait, ts_aqr, ts_rel;

    // Construct and save the primary log message for the insert operation
    char msg[64];
    snprintf(msg, sizeof(msg), "INSERT,%d,%s,%d", hash, name, salary);
    PrintLog(GetMicroTime(), priority, msg);

    // Set up rwlock and log
    ts_wait= GetMicroTime();
    rwlock_acquire_writelock(lock);
    ts_aqr = GetMicroTime();
    // Simulate waiting & awakening
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    PrintLog(ts_aqr, priority, "AWAKENED FOR WORK");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    // Set up for traversal over hash table
    hashRecord *head = table->head;
    hashRecord *curr = head;
    hashRecord *prev = NULL;

    while(curr != NULL){ // Navigate to a either a duplicate node or the end of the list

        if(curr->hash == hash){ // Found duplicate
            break;
        }

        prev = curr;
        curr = curr->next;
    }
    
    if(curr != NULL){ // Node already existed
        curr->salary = salary;
    }
    else{ // New node should be made

        command->succeeded = true;

        // Create space for new node
        if((curr = malloc(sizeof(hashRecord))) == NULL){
            fprintf(stderr, "Error: malloc failed");
            exit(1);
        }

        // Copy info over
        curr->hash = hash;
        strcpy(curr->name, name);
        curr->salary = salary;

        // Update prev/next/head if needed
        curr->next = NULL;
        if(prev != NULL) prev->next = curr;
        if(head == NULL) table->head = curr;
    }

    // Save info in command for remaining logging
    command->record = curr;

    PrintUpdate(command);

    // Log and release lock
    ts_rel = GetMicroTime();
    rwlock_release_writelock(lock);
    PrintLog(ts_rel, priority, "WRITE LOCK RELEASED");

}

int delete(char name[]){
    return 0;
}

int updateSalary(char name[], uint32_t salary){
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

    // Sort by hash
    int cmp(const void *a, const void *b) {
        hashRecord *ha = *(hashRecord**)a;
        hashRecord *hb = *(hashRecord**)b;
        return (ha->hash > hb->hash) - (ha->hash < hb->hash);
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