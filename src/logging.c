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
                printf("Inserted %d,%s,%u\n", record->hash, name, salary);
            } else {
                printf("Insert failed.  Entry %d is a duplicate.\n", record->hash);
            }
            break;
            
        case OP_UPDATE:
            if(command->succeeded) {
                printf("Updated record %d from %d,%s,%u to %d,%s,%u\n", 
                       record->hash, record->hash, name, command->oldSalary,
                       record->hash, name, salary);
            } else {
                int32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));
                printf("Update failed. Entry %d not found.\n", hash);
            }
            break;
            
        case OP_DELETE:
            if(command->succeeded) {
                printf("Deleted record for %d,%s,%u\n", record->hash, name, salary);
            } else {
                printf("Delete failed. Entry not found.\n");
            }
            break;
            
        case OP_SEARCH:
            if(command->succeeded) {
                printf("Found: %d,%s,%u\n", record->hash, name, salary);
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

void PrintResults(){}