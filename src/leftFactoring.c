#include "leftFactoring.h"
#include "utils.h"

/**
 * Performs left factoring on the global grammar.
 *
 * This function iterates over each rule in the grammar and identifies common prefixes
 * among the productions of a non-terminal. If a common prefix is found, it replaces
 * the common part with a new non-terminal and adds the remaining production as a rule
 * for the new non-terminal. This process helps in transforming the grammar into a form
 * suitable for LL(1) parsing by removing left recursion.
 *
 * The function modifies the global `grammar` array and updates `numRules` to reflect
 * the new rules added during the left factoring process.
 */

void leftFactorGrammar() {
    printf("\nPerforming Left Factoring...\n");

    for (int i = 0; i < numRules; i++) {
        bool prev[MAX_PRODUCTIONS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        int c[128] = {0};  // Changed from char c[128] to int c[128]
        char ch;
        int j = 0, maxCount;

        while (true) {
            maxCount = 0;
            for (int k = 0; k < grammar[i].count; k++) {
                if (prev[k] && strlen(grammar[i].productions[k]) > j) {
                    unsigned char idx = (unsigned char)grammar[i].productions[k][j];  // Cast to unsigned char
                    c[idx]++;
                }
            }

            for (int k = 0; k < 128; k++) {
                if (c[k] > maxCount) {
                    maxCount = c[k];
                    ch = (char)k;
                }
                c[k] = 0;
            }

            if (maxCount < 2) break;

            for (int k = 0; k < grammar[i].count; k++)
                prev[k] = j < strlen(grammar[i].productions[k]) && grammar[i].productions[k][j] == ch;
            j++;
        }

        if (j == 0) continue;

        char newNonTerminal = 'A' + numRules;
        grammar[numRules].nonTerminal = newNonTerminal;
        grammar[numRules].count = 0;

        int uniqueIndex = 0;
        char uniqueProductions[MAX_PRODUCTIONS][MAX_LENGTH_PRODUCTION] = {0};

        for (int k = 0; k < grammar[i].count; k++) {
            if (prev[k] && grammar[i].productions[k][0] != '\0') {
                int isDuplicate = 0;
                for (int p = 0; p < uniqueIndex; p++) {
                    if (strcmp(uniqueProductions[p], grammar[i].productions[k] + j) == 0) {
                        isDuplicate = 1;
                        break;
                    }
                }
                if (!isDuplicate) {
                    strcpy(uniqueProductions[uniqueIndex++], grammar[i].productions[k] + j);
                    strcpy(grammar[numRules].productions[grammar[numRules].count++], grammar[i].productions[k] + j);
                }

                grammar[i].productions[k][j] = newNonTerminal;
                grammar[i].productions[k][j + 1] = '\0';
            }
        }

        numRules++;
    }

    printf("\nLeft Factoring Completed.\n");
}