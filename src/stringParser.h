#ifndef STRING_PARSER_H
#define STRING_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsingStack.h"

// Define the LineErrors structure
typedef struct {
    int lineNumber;
    int count;
    char messages[10][100]; // Up to 10 error messages per line
} LineErrors;

// Function prototypes
int parseString(char* inputString, LineErrors* lineErrors);
void parseInputFile(const char* filename);

#endif