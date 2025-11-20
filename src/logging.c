#include "logging.h"
#include "common.h"

void PrintUpdate(CommandInfo *command){

    OperationType op = command->op;
    char *name = command->name;
    uint32_t salary = command->salary;
    hashRecord *record = command->record;

    // Format and print output based on the completed operation
    switch(op){

        case OP_INSERT:
            if(command->succeeded) {
                printf("Inserted %u,%s,%u\n", record->hash, name, salary);
            } else {
                printf("Insert failed.  Entry %u is a duplicate.\n", record->hash);
            }
            break;
            
        case OP_UPDATE:
            if(command->succeeded) {
                printf("Updated record %u from %u,%s,%u to %u,%s,%u\n", 
                       record->hash, record->hash, name, command->oldSalary,
                       record->hash, name, salary);
            } else {
                int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));
                printf("Update failed. Entry %d not found.\n", hash);
            }
            break;
            
        case OP_DELETE:
            if(command->succeeded) {
                printf("Deleted record for %u,%s,%u\n", record->hash, name, salary);
            } else {
                printf("Delete failed. Entry not found.\n");
            }
            break;
            
        case OP_SEARCH:
            if(command->succeeded) {
                printf("Found: %u,%s,%u\n", record->hash, name, salary);
            } else {
                printf("%s not found.\n", name);
            }
            break;

        case OP_PRINT: 
            // Print operation doesn't output here
            break;
            
        default:
            fprintf(stderr, "Error: incorrect call to PrintOutputs\n");
            exit(1);
    }
}

void PrintLog(long long timestamp, int priority, const char *message){

    FILE *fl;

    if((fl = fopen("hash.log","a")) == NULL){
        fprintf(stderr, "Error: failed opening log file");
        exit(1);
    }

    fprintf(fl, "%lld: THREAD %d %s\n", timestamp, priority, message);

    fclose(fl);
}

int cmp(const void *a, const void *b) {
    hashRecord *ha = *(hashRecord**)a;
    hashRecord *hb = *(hashRecord**)b;
    return (ha->hash > hb->hash) - (ha->hash < hb->hash);
}

void PrintResults(HashTable *table) {
    printf("Current Database:\n");
    int count = 0;
    hashRecord *curr = table->head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    
    hashRecord **arr = malloc(sizeof(hashRecord*) * count);
    curr = table->head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }

    qsort(arr, count, sizeof(hashRecord*), cmp);

    for (int i = 0; i < count; i++) {
        printf("%u,%s,%u\n", arr[i]->hash, arr[i]->name, arr[i]->salary);
    }

    free(arr);
}