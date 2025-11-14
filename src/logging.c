#include "logging.h"

// Outputs command completion info to the console
void PrintUpdate(CommandInfo *command){

    OperationType op = command->op;
    char *name = command->name;
    uint32_t salary = command->salary;
    hashRecord *record = command->record;

    // Format and print output based on the completed operation
    switch(op){

        case OP_INSERT:
            if(command->succeeded) printf("Inserted %s, %d\n", name, salary);
            else printf("Insert failed.  Entry %d is a duplicate.\n", record->hash);
            break;
            
        case OP_UPDATE:
            // to be filled
        case OP_DELETE:
            // to be filled
        case OP_SEARCH:
            if(command->succeeded) printf("Found: %s, %d\n", name, salary);
            else printf("%s not found.\n", name);
            break;

        case OP_PRINT: 
            //maybe call this repeatedly in Print() function
        default:
            fprintf(stderr, "Error: incorrect call to PrintOutputs\n");
            exit(1);
    }
}

// Outputs log info to the hash.log file
void PrintLog(long long timestamp, int priority, const char *message){

    FILE *fl;

    if((fl = fopen("hash.log","a")) == NULL){
        fprintf(stderr, "Error: failed opening log file");
        exit(1);
    }

    // This expects that the calling function handles the dynamic message
    fprintf(fl, "%lld: THREAD %d %s\n", timestamp, priority, message);

    fclose(fl);
}

void PrintResults(){}