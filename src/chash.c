// Main program that reads the commands.txt and produces output to the console

#include <stdio.h>
#include <string.h>

#include "hashtable.h"
#include "rwlock.h"
#include "types.h"
#include "logging.h"


// NOTE: We still need a command parser and error/validity checks for each input (name length, etc)
int main(int argc, char *argv[]){

    // Set the hashtable up
    HashTable table;
    rwlock_init(&table.lock);
    table.head = NULL;

    // This cmd and file should be updated with each subsequent command
    CommandInfo cmd;
    FILE *fo;

    // Set up output.txt
    if((fo = fopen("output.txt","w")) == NULL){
        fprintf(stderr, "Error: failed to open output file");
        exit(1);
    }

    // Ideally the runner would collect each command and set up
    // each cmd and pass them to their respective functions.
    // I included a insert() and search() call to show the calls.
    // After each command, you can use PrintOutputs().

    cmd.op = OP_INSERT;
    cmd.salary = 40000;
    strcpy(cmd.name, "Foo Bar");
    cmd.priority = 1;

    insert(&table, &cmd);
    PrintOutputs(&cmd, fo);

    cmd.op = OP_SEARCH;
    strcpy(cmd.name, "Foo Bar");
    cmd.priority = 2;

    search(&table, &cmd);
    PrintOutputs(&cmd, fo);

    freeTable(&table);
    fclose(fo);

    return 0;
}
