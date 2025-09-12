#include "LL1Parser.h"
#include "utils.h"

/**
 * Computes the first set of a given non-terminal.
 *
 * The first set of a non-terminal is the set of terminals that can
 * appear as the first symbol of any string derived from that
 * non-terminal.
 *
 * @param nonTerminal The non-terminal to compute the first set for.
 */
void computeFirstSet(char nonTerminal) {
    int index = getIndex(nonTerminal);
    if (index == -1 || firstCount[index] > 0) return;

    for (int i = 0; i < grammar[index].count; i++) {
        char *production = grammar[index].productions[i];

        for (int j = 0; production[j] != '\0'; j++) {
            char symbol = production[j];

            if (isTerminal(symbol)) {
                addToSet(firstSet[index], symbol, &firstCount[index]);
                break;
            } else {
                int nextIndex = getIndex(symbol);
                if (nextIndex != -1) {
                    if (firstCount[nextIndex] == 0) computeFirstSet(symbol);

                    for (int k = 0; k < firstCount[nextIndex]; k++) {
                        if (firstSet[nextIndex][k] != '#') {
                            addToSet(firstSet[index], firstSet[nextIndex][k], &firstCount[index]);
                        }
                    }
                    if (!containsEpsilon(firstSet[nextIndex], firstCount[nextIndex])) break;
                }
            }
        }
    }
}

/**
 * Computes the follow set of a given non-terminal.
 *
 * The follow set of a non-terminal is the set of terminals that can 
 * appear immediately to the right of that non-terminal in some 
 * "sentential" form. This function fills the `followSet` array with 
 * the terminals that can follow the specified non-terminal, according 
 * to the grammar rules.
 *
 * It starts by adding the end-of-input symbol '$' to the follow set 
 * of the start symbol. For each rule in the grammar, it processes 
 * each production to determine if the non-terminal occurs, and if so, 
 * what symbols can follow it. It uses the first set of the following 
 * symbols to populate the follow set, and if necessary, recursively 
 * computes the follow set of other non-terminals to include their 
 * follow sets.
 *
 * @param nonTerminal The non-terminal to compute the follow set for.
 */

 void computeFollowSet(char nonTerminal) {
    int index = getIndex(nonTerminal);
    if (index == -1) return;
    
    // Avoid infinite recursion
    if (followComputed[index]) return;
    followComputed[index] = 1;

    printf("Computing FOLLOW(%c)\n", nonTerminal);

    // Step 1: Add $ to the follow set of the start symbol
    if (nonTerminal == grammar[0].nonTerminal) {
        addToSet(followSet[index], '$', &followCount[index]);
    }

    // Step 2: For each production A -> αBβ, add FIRST(β) - {ε} to FOLLOW(B)
    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < grammar[i].count; j++) {
            char *production = grammar[i].productions[j];
            int len = strlen(production);

            for (int k = 0; k < len; k++) {
                if (production[k] == nonTerminal) {
                    // Found the non-terminal in the production
                    printf("  Processing production: %s\n", production);

                    if (k + 1 < len) {
                        // There is something after the non-terminal
                        int hasEpsilon = 1;
                        for (int m = k + 1; m < len && hasEpsilon; m++) {
                            char nextChar = production[m];
                            hasEpsilon = 0;

                            if (isTerminal(nextChar)) {
                                printf("    Adding terminal to FOLLOW(%c): %c\n", nonTerminal, nextChar);
                                addToSet(followSet[index], nextChar, &followCount[index]);
                                break;
                            } else {
                                int nextIndex = getIndex(nextChar);
                                if (nextIndex != -1) {
                                    // Compute FIRST set if needed
                                    if (firstCount[nextIndex] == 0) {
                                        computeFirstSet(nextChar);
                                    }
                                    
                                    // Add FIRST(nextChar) - {ε} to FOLLOW(nonTerminal)
                                    for (int l = 0; l < firstCount[nextIndex]; l++) {
                                        if (firstSet[nextIndex][l] != '#') {
                                            printf("    Adding from FIRST(%c) to FOLLOW(%c): %c\n", 
                                                  nextChar, nonTerminal, firstSet[nextIndex][l]);
                                            addToSet(followSet[index], firstSet[nextIndex][l], &followCount[index]);
                                        }
                                    }

                                    // If FIRST(nextChar) contains ε, continue to the next symbol
                                    hasEpsilon = containsEpsilon(firstSet[nextIndex], firstCount[nextIndex]);
                                }
                            }
                        }

                        // If all symbols after nonTerminal can derive ε, add FOLLOW(A) to FOLLOW(nonTerminal)
                        if (hasEpsilon) {
                            printf("    Adding FOLLOW(%c) to FOLLOW(%c) (all derive epsilon)\n", 
                                  grammar[i].nonTerminal, nonTerminal);
                            computeFollowSet(grammar[i].nonTerminal);
                            int A_index = getIndex(grammar[i].nonTerminal);
                            for (int l = 0; l < followCount[A_index]; l++) {
                                addToSet(followSet[index], followSet[A_index][l], &followCount[index]);
                            }
                        }
                    } else {
                        // The non-terminal is at the end of the production
                        // Add FOLLOW(A) to FOLLOW(nonTerminal)
                        if (grammar[i].nonTerminal != nonTerminal) {  // Avoid infinite recursion
                            printf("    Adding FOLLOW(%c) to FOLLOW(%c) (end of production)\n", 
                                  grammar[i].nonTerminal, nonTerminal);
                            computeFollowSet(grammar[i].nonTerminal);
                            int A_index = getIndex(grammar[i].nonTerminal);
                            for (int l = 0; l < followCount[A_index]; l++) {
                                addToSet(followSet[index], followSet[A_index][l], &followCount[index]);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Constructs an LL(1) parsing table based on the first and follow sets of each non-terminal.
 *
 * The table is a 2D array of strings, indexed by non-terminal and terminal. The value at
 * each index is the production to be used when parsing the input string. If a given
 * non-terminal can produce the empty string, the table will also contain an entry for
 * the follow set of the non-terminal.
 */
void constructLL1ParsingTable() {
    // Initialize the parsing table with empty strings
    for (int i = 0; i < MAX_RULES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            parsingTable[i][j][0] = '\0';
        }
    }

    for (int i = 0; i < numRules; i++) {
        
        for (int j = 0; j < grammar[i].count; j++) {
            char *production = grammar[i].productions[j];
            
            // Handle epsilon production separately
            if (strcmp(production, "#") == 0) {
                for (int k = 0; k < followCount[i]; k++) {
                    char terminal = followSet[i][k];
                    unsigned char idx = (unsigned char)terminal;
                    strcpy(parsingTable[i][idx], "#");
                }
                continue;
            }
            
            // The first character of the production
            char firstChar = production[0];
            
            if (isTerminal(firstChar) && firstChar != '#') {
                // Direct terminal - add to parsing table
                unsigned char idx = (unsigned char)firstChar;
                strcpy(parsingTable[i][idx], production);
            } else if (!isTerminal(firstChar)) {
                // Non-terminal - use its FIRST set
                int firstIndex = getIndex(firstChar);
                if (firstIndex != -1) {
                    // Add entries for all terminals in FIRST(firstChar)
                    for (int k = 0; k < firstCount[firstIndex]; k++) {
                        char terminal = firstSet[firstIndex][k];
                        if (terminal != '#') {
                            unsigned char idx = (unsigned char)terminal;
                            strcpy(parsingTable[i][idx], production);
                        }
                    }
                    
                    // If FIRST set contains epsilon, add entries for FOLLOW(nonTerminal)
                    if (containsEpsilon(firstSet[firstIndex], firstCount[firstIndex])) {
                        for (int k = 0; k < followCount[i]; k++) {
                            char terminal = followSet[i][k];
                            unsigned char idx = (unsigned char)terminal;
                            if (parsingTable[i][idx][0] == '\0') { // Only if not already set
                                strcpy(parsingTable[i][idx], production);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Prints the first sets for each non-terminal in the grammar.
 *
 * This function iterates over all the grammar rules and prints the first set 
 * associated with each non-terminal. The first set contains the set of terminals
 * that can appear as the first symbol in strings derived from the non-terminal.
 * The output is formatted as "FIRST(X) = { ... }", where X is the non-terminal.
 */

void printFirstSets() {
    printf("\nFirst Sets:\n");
    for (int i = 0; i < numRules; i++) {
        printf("FIRST(%c) = { ", grammar[i].nonTerminal);
        for (int j = 0; j < firstCount[i]; j++) {
            firstSet[i][j] == 'i'
            ? printf("id ")
            : printf("%c ", firstSet[i][j]);
        }
        printf("}\n");
    }
}

/**
 * Prints the follow sets for each non-terminal in the grammar.
 *
 * This function iterates over all the grammar rules and prints the follow set 
 * associated with each non-terminal. The follow set contains the set of terminals
 * that can appear immediately to the right of the non-terminal in some sentential
 * form. The output is formatted as "FOLLOW(X) = { ... }", where X is the non-terminal.
 */
void printFollowSets() {
    printf("\nFollow Sets:\n");
    for (int i = 0; i < numRules; i++) {
        printf("FOLLOW(%c) = { ", grammar[i].nonTerminal);
        for (int j = 0; j < followCount[i]; j++) {
            printf("%c ", followSet[i][j]);
        }
        printf("}\n");
    }
}

/**
 * Displays the LL(1) parsing table.
 *
 * The parsing table is a 2D array of strings, indexed by non-terminal and terminal.
 * The value at each index is the production to be used when parsing the input string.
 * If a given non-terminal can produce the empty string, the table will also contain
 * an entry for the follow set of the non-terminal.
 *
 * The table is displayed with the non-terminals along the left side and the terminals
 * along the top. The intersection of a non-terminal and a terminal contains the
 * production to be used when parsing the input string. If a given non-terminal and
 * terminal do not correspond to any production, the table will contain a '-' in
 * that position.
 */
void displayParsingTable() {
    printf("\nLL(1) Parsing Table:\n");

    char terminals[MAX_SYMBOLS];
    int termCount = 0;

    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < firstCount[i]; j++) {
            char terminal = firstSet[i][j];
            if (terminal != '#' && isTerminal(terminal)) {
                addToSet(terminals, terminal, &termCount);
            }
        }
        for (int j = 0; j < followCount[i]; j++) {
            char terminal = followSet[i][j];
            if (isTerminal(terminal)) {
                addToSet(terminals, terminal, &termCount);
            }
        }
    }

    addToSet(terminals, '$', &termCount);

    printf("  \t");
    for (int i = 0; i < termCount; i++) {
        terminals[i] == 'i' ? printf("id\t") :
        printf("%c\t", terminals[i]);
    }
    printf("\n");

    for (int i = 0; i < numRules; i++) {
        printf("%c\t", grammar[i].nonTerminal);

        for (int j = 0; j < termCount; j++) {
            char terminal = terminals[j];
            // Use unsigned char for array index
            unsigned char idx = (unsigned char)terminal;

            if (parsingTable[i][idx][0] != '\0') {
                printf("%s\t", parsingTable[i][idx]);
            } else {
                printf("-\t");
            }
        }
        printf("\n");
    }
    printf("\n");
}