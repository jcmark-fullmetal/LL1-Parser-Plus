#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "leftFactoring.h"
#include "leftRecursion.h"
#include "LL1Parser.h"
#include "stringParser.h"

/**
 * Main entry point of the program.
 *
 * Reads a grammar from a file, removes left factoring and left recursion, computes
 * the first and follow sets, constructs an LL(1) parsing table and displays the
 * results. Then, it reads input strings and parses them using the LL(1) parser.
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        printf("Where <input_file> contains the strings to be parsed.\n");
        return 1;
    }
    
    // Initialize the parsing table with empty strings
    for (int i = 0; i < MAX_RULES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            parsingTable[i][j][0] = '\0';
        }
    }

    readGrammarFromFile();
    printf("Original Grammar:\n");
    displayGrammar();

    printf("\nProcessing Left Factoring...\n");
    leftFactorGrammar();
    displayGrammar();

    printf("\nProcessing Left Recursion Removal...\n");
    removeLeftRecursion();
    displayGrammar();
    
    printf("\nComputing First & Follow Sets...\n");
    for (int i = 0; i < numRules; i++) {
        computeFirstSet(grammar[i].nonTerminal);
    }
    
    // First process the full grammar
    for (int i = 0; i < numRules; i++) {
        computeFollowSet(grammar[i].nonTerminal);
    }

    printf("\nConstructing LL(1) Parsing Table...\n");
    constructLL1ParsingTable();

    // Remove spaces before displaying results and parsing
    removeSpaceFromGrammar();
    
    printFirstSets();
    printFollowSets();
    displayParsingTable();
    
    // Parse input strings
    parseInputFile(argv[1]);

    return 0;
}