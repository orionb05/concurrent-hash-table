#ifndef LOGGING_H
#define LOGGING_H

/*
// Handles all stdout, output.txt, and hash.log printing operations.
// General output is handled by PrintOutputs, PrintLog handles hash.log. 
//
// These methods work directly with parsed commands and should be called
// by both the table methods and the runner as needed.
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "types.h"

void PrintOutputs(CommandInfo *command, FILE *fo);

void PrintLog(long long timestamp, int priority, const char *message);

#endif