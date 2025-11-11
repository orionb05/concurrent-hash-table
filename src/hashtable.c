#include "hashtable.h"
#include "common.h"
#include "types.h"

int insert(HashTable *table, CommandInfo *command){
    
    const char *name = command->name;
    uint32_t salary = command->salary;
    int priority = command->priority;

    if(strlen(name) >49){
        fprintf(stderr, "Error: failed to allocate memory for node, name is too large\n");
        exit(1);
    }

    int32_t hash = jenkins_one_at_a_time_hash(name, strlen(name));

    rwlock_t *lock = table->lock;
    long long timestamp;

    timestamp = rwlock_acquire_writelock(lock);
    PrintLog(timestamp, priority, "WRITE LOCK ACQUIRED");

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

    timestamp = rwlock_release_writelock(lock);
    PrintLog(timestamp, priority, "WRITE LOCK RELEASED");

    // Construct and save the log message for the insert operation
    char msg[64];
    snprintf(msg, sizeof(msg), "INSERT,%d,%s,%d", hash, name, salary);
    PrintLog(GetMicroTime, priority, msg);

    return 1;
}

int delete(char name[]){
    return 0;
};

int updateSalary(char name[], uint32_t salary){
    return 0;
};

int search(char name[]){
    return 0;
};

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