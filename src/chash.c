// Main program that reads the commands.txt and produces output to the console
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "hashtable.h"
#include "rwlock.h"
#include "types.h"
#include "logging.h"
#include "common.h"

// Use cmp from hashtable.c
extern int cmp(const void *a, const void *b);

// Global variables
HashTable table;
int numThreads = 0;
int readLockCount = 0;
int writeLockCount = 0;

int main(int argc, char *argv[]) {
    // Initialize hash table
    rwlock_init(&table.lock);
    table.head = NULL;
    
    // Reset the log file
    FILE *logFile = fopen("hash.log", "w");
    if (logFile == NULL) {
        perror("Error: failed to reset log file");
        exit(1);
    }
    fclose(logFile);
    
    // Open commands file
    FILE *cmdFile = fopen("commands.txt", "r");
    if (cmdFile == NULL) {
        perror("Error: failed to open commands.txt");
        exit(1);
    }
    
    // Read first line to get number of threads
    char line[256];
    if (fgets(line, sizeof(line), cmdFile) == NULL) {
        fprintf(stderr, "Error: failed to read thread count\n");
        exit(1);
    }
    
    // Parse thread count line: threads,60,0
    char dummy[20];
    int dummyVal;
    sscanf(line, "%[^,],%d,%d", dummy, &numThreads, &dummyVal);
    
    // Read and execute all commands in order
    while (fgets(line, sizeof(line), cmdFile) != NULL) {
        CommandInfo cmd = {0};
        
        // Parse the command line
        char operation[20];
        char name[50];
        int salary, threadId;
        
        // Format: operation,name,salary,threadId
        if (sscanf(line, "%[^,],%[^,],%d,%d", operation, name, &salary, &threadId) == 4) {
            strcpy(cmd.name, name);
            cmd.salary = salary;
            cmd.priority = threadId;
            
            // Track lock counts
            if (strcmp(operation, "search") == 0 || strcmp(operation, "print") == 0) {
                readLockCount++;
            } else {
                writeLockCount++;
            }
            
            // Determine operation type and execute
            if (strcmp(operation, "insert") == 0) {
                cmd.op = OP_INSERT;
                insert(&table, &cmd);
            } else if (strcmp(operation, "delete") == 0) {
                cmd.op = OP_DELETE;
                delete(&table, &cmd);
            } else if (strcmp(operation, "update") == 0) {
                cmd.op = OP_UPDATE;
                updateSalary(&table, &cmd);
            } else if (strcmp(operation, "search") == 0) {
                cmd.op = OP_SEARCH;
                search(&table, &cmd);
            } else if (strcmp(operation, "print") == 0) {
                cmd.op = OP_PRINT;
                printTable(&table, &cmd);
            }
        }
    }
    
    fclose(cmdFile);
    
    // Open log file for appending final statistics
    FILE *logFile2 = fopen("hash.log", "a");
    if (logFile2 == NULL) {
        perror("Error: failed to open log file");
        exit(1);
    }
    
    // Print final statistics to both stdout and log file
    printf("\nNumber of lock acquisitions: %d\n", readLockCount + writeLockCount);
    printf("Number of lock releases: %d\n", readLockCount + writeLockCount);
    printf("Final Table:\n");
    
    fprintf(logFile2, "\nNumber of lock acquisitions: %d\n", readLockCount + writeLockCount);
    fprintf(logFile2, "Number of lock releases: %d\n", readLockCount + writeLockCount);
    fprintf(logFile2, "Final Table:\n");
    
    // Print final table (sorted)
    int count = 0;
    hashRecord *curr = table.head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    
    hashRecord **arr = malloc(sizeof(hashRecord*) * count);
    curr = table.head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    qsort(arr, count, sizeof(hashRecord*), cmp);
    
    for (int i = 0; i < count; i++) {
        printf("%d,%s,%u\n", arr[i]->hash, arr[i]->name, arr[i]->salary);
        fprintf(logFile2, "%d,%s,%u\n", arr[i]->hash, arr[i]->name, arr[i]->salary);
    }
    
    fclose(logFile2);
    free(arr);
    freeTable(&table);
    
    return 0;
}