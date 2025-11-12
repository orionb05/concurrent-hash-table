// Main program that reads the commands.txt and produces output to the console

#include <stdio.h>
#include <string.h>

#include "hashtable.h"
#include "rwlock.h"
#include "types.h"
#include "logging.h"

int main(int argc, char *argv[]){

    puts("Hello World!");

    HashTable table;
    rwlock_init(&table.lock);
    table.head = NULL;

    CommandInfo cmd;
    FILE *fo;

    // Open output.txt
    if((fo = fopen("output.txt","w")) == NULL){
        fprintf(stderr, "Error: failed to open output file");
        exit(1);
    }

    cmd.op = OP_INSERT;
    cmd.salary = 40000;
    strcpy(cmd.name, "Foo Bar");
    cmd.priority = 1;

    insert(&table, &cmd);
    PrintOutputs(&cmd, fo);

    freeTable(&table);

    fclose(fo);

    return 0;
}
