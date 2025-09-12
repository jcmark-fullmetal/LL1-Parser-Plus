#include "leftRecursion.h"
#include "utils.h"

/**
 * Removes left recursion from the global grammar.
 *
 * This function iterates over each rule in the grammar, identifying and eliminating
 * left recursion. For a grammar rule with left recursion, it separates the productions
 * into two groups: those that begin with the non-terminal (alpha) and those that do not
 * (beta). It then constructs new rules to eliminate the left recursion by introducing
 * a new non-terminal.
 *
 * The function modifies the global `grammar` array and updates `numRules` to reflect
 * the new rules added during the left recursion removal process.
 */

 void removeLeftRecursion() {
    printf("\nPerforming Left Recursion Removal...\n");

    for (int i = 0; i < numRules; i++) {
        char alpha[MAX_PRODUCTIONS][MAX_LENGTH_PRODUCTION], beta[MAX_PRODUCTIONS][MAX_LENGTH_PRODUCTION];
        int alphaCount = 0, betaCount = 0;
        char nonTerminal = grammar[i].nonTerminal;

        for (int j = 0; j < grammar[i].count; j++) {
            char *production = grammar[i].productions[j];

            if (production[0] == nonTerminal) {
                strcpy(alpha[alphaCount++], production + 1);
            } else {
                strcpy(beta[betaCount++], production);
            }
        }

        if (alphaCount > 0) {
            // Generate a new unique non-terminal
            char newNonTerminal = findAvailableNonTerminal();

            grammar[i].count = 0;
            int uniqueIndex = 0;
            char uniqueProductions[MAX_PRODUCTIONS][MAX_LENGTH_PRODUCTION] = {0};

            // Update beta productions with the new non-terminal
            for (int j = 0; j < betaCount; j++) {
                // Check for duplicates
                int isDuplicate = 0;
                for (int p = 0; p < uniqueIndex; p++) {
                    if (strcmp(uniqueProductions[p], beta[j]) == 0) {
                        isDuplicate = 1;
                        break;
                    }
                }
                if (!isDuplicate) {
                    strcpy(uniqueProductions[uniqueIndex++], beta[j]);
                    sprintf(grammar[i].productions[grammar[i].count++], "%s%c", beta[j], newNonTerminal);
                }
            }

            // Add new rule for the new non-terminal
            grammar[numRules].nonTerminal = newNonTerminal;
            grammar[numRules].count = 0;
            uniqueIndex = 0;
            memset(uniqueProductions, 0, sizeof(uniqueProductions));

            for (int j = 0; j < alphaCount; j++) {
                int isDuplicate = 0;
                for (int p = 0; p < uniqueIndex; p++) {
                    if (strcmp(uniqueProductions[p], alpha[j]) == 0) {
                        isDuplicate = 1;
                        break;
                    }
                }
                if (!isDuplicate) {
                    strcpy(uniqueProductions[uniqueIndex++], alpha[j]);
                    sprintf(grammar[numRules].productions[grammar[numRules].count++], "%s%c", alpha[j], newNonTerminal);
                }
            }
            strcpy(grammar[numRules].productions[grammar[numRules].count++], "#"); // Epsilon

            numRules++;
        }
    }

    printf("\nLeft Recursion Removal Completed.\n");
}