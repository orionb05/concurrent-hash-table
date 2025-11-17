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

    // Reset the log file
    FILE *logFile = fopen("hash.log", "w");
    if (logFile == NULL) {
        perror("Error : failed to reset log file");
        exit(1);
    }
    fclose(logFile);

    // This cmd should be updated with each subsequent command
    CommandInfo cmd = {0};

    // We should def make a InitCommand() function to *reset*, *validate(error-wise)*, and *fill* each command.

    // Ideally the runner would collect each command and set up
    // each cmd and pass them to their respective functions.
    // I included an insert() and search() call to show the calls.

    cmd.op = OP_INSERT;
    cmd.salary = 40000;
    strcpy(cmd.name, "Foo Bar");
    cmd.priority = 1;

    insert(&table, &cmd);

    cmd.op = OP_INSERT;
    cmd.salary = 25620;
    strcpy(cmd.name, "Choclate");
    cmd.priority = 2;
    insert(&table, &cmd);

    cmd.op = OP_INSERT;
    cmd.salary = 2637214;
    strcpy(cmd.name, "Ice cream");
    cmd.priority = 3;
    insert(&table, &cmd);

    cmd.op = OP_INSERT;
    cmd.salary = 2637214;
    strcpy(cmd.name, "Dot cream");
    cmd.priority = 3;
    insert(&table, &cmd);

    cmd = (CommandInfo){0};   
    cmd.op = OP_SEARCH;
    strcpy(cmd.name, "Foo Bar");
    cmd.priority = 2;
    search(&table, &cmd);

    cmd = (CommandInfo){0};
    cmd.op = OP_PRINT;
    cmd.priority = 3;
    printTable(&table, &cmd);

    //At the end, we need to do the final print outlined in webcourses->files->pa2->output.txt(bottom)

    freeTable(&table);

    return 0;
}
