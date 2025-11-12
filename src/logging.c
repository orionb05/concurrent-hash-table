#include "logging.h"

// Outputs command completion info to output.txt and console
void PrintOutputs(CommandInfo *command, FILE *fo){

    OperationType op = command->op;
    char name[50];
    strcpy(name, command->name);
    uint32_t salary = command->salary;

    // Format and print output based on the completed operation
    // TO BE FILLED DURING OTHER FUNCTIONS
    switch(op){

        case OP_INSERT:
            printf("Inserted %s, %d\n", name, salary);
            fprintf(fo, "Inserted %s, %d\n", name, salary);
            break;
            
        case OP_UPDATE:
        case OP_DELETE:

        case OP_SEARCH:
            // Salary is set to '-404' in search function if search fails
            if(command->salary == -404){
                printf("No Record Found\n");
                fprintf(fo, "Inserted %s, %d\n", name, salary);
            }
            else{
                printf("Found %s. Salary: %d\n", name, salary);
                fprintf(fo, "Found %s. Salary: %d\n", name, salary);
            }
            break;

        case OP_PRINT: //maybe call this repeatedly in Print() function
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
    fprintf(fl, "%lld: THREAD %d, %s\n", timestamp, priority, message);

    fclose(fl);
}
