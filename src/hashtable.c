#include "hashtable.h"
#include "common.h"
#include "types.h"
#include "logging.h"


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
    if((ts_aqr - ts_wait) > 10) PrintLog(ts_wait, priority, "WAITING FOR MY TURN");
    PrintLog(ts_aqr, priority, "WRITE LOCK ACQUIRED");

    // Set up for traversal over hash table
    hashRecord *head = table->head;
    hashRecord *curr = head;
    hashRecord *prev = NULL;

    // Navigate to a either a duplicate node or the end of the list
    while(curr != NULL){

        if(curr->hash == hash){
            break;
        }

        prev = curr;
        curr = curr->next;
    }

    // Entry already exists
    if(curr != NULL){
        curr->salary = salary;
    }
    // Entry doesn't exist
    else{
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
    PrintLog(ts_aqr, priority, "READ LOCK ACQUIRED");

    // Set up for traversal over hash table
    hashRecord *curr = table->head;

    // Navigate to either the query node or the end of the list
    while(curr != NULL){

        if(curr->hash == hash) break;

        curr = curr->next;
    }

    hashRecord* res = curr;

    // If search failed, use this to later notify logger
    if(res == NULL) command->salary = -404;

    // Log and release lock
    ts_rel = GetMicroTime();
    rwlock_release_readlock(lock);
    PrintLog(ts_rel, priority, "READ LOCK RELEASED");

    return res;
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