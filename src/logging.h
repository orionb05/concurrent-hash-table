#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include <stdio.h>

#include "types.h"

/*
// Handles all stdout, output.txt, and hash.log printing operations.
// General output is handled by PrintOutputs while logging follows. 
*/

void PrintOutputs(CommandInfo *command, FILE *fo){}

void PrintLog(long long timestamp, int priority, const char *message){}

#endif