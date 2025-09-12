#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_RULES 100
#define MAX_PRODUCTIONS 10
#define MAX_LENGTH_PRODUCTION 50
#define MAX_SYMBOLS 20

typedef struct {
    char nonTerminal;
    char productions[MAX_PRODUCTIONS][MAX_LENGTH_PRODUCTION];
    int count;
} Rule;

// Declare the global variables as extern
extern Rule grammar[MAX_RULES];
extern int numRules;

extern char firstSet[MAX_RULES][MAX_SYMBOLS];
extern int firstCount[MAX_RULES];

extern char followSet[MAX_RULES][MAX_SYMBOLS];
extern int followCount[MAX_RULES];
extern int followComputed[MAX_RULES];

extern char parsingTable[MAX_RULES][MAX_SYMBOLS][MAX_LENGTH_PRODUCTION];

// Function prototypes
void readGrammarFromFile();
void displayGrammar();
void addSpaces(char *str);
void addSpacesToGrammar();
void removeSpacesFromString(char *input, char *output);
void removeSpaceFromGrammar();
int isTerminal(char symbol);
int getIndex(char symbol);
void addToSet(char *set, char symbol, int *count);
int containsEpsilon(char *set, int count);
char findAvailableNonTerminal();
#endif // UTILS_H