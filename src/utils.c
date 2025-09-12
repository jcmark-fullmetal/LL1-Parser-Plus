#include "utils.h"

// Define the global variables
Rule grammar[MAX_RULES];
int numRules = 0;

char firstSet[MAX_RULES][MAX_SYMBOLS];
int firstCount[MAX_RULES] = {0};

char followSet[MAX_RULES][MAX_SYMBOLS];
int followCount[MAX_RULES] = {0};
int followComputed[MAX_RULES] = {0};

char parsingTable[MAX_RULES][MAX_SYMBOLS][MAX_LENGTH_PRODUCTION];

/**
 * Removes all spaces from the file specified by inputFile and writes the result to
 * the file specified by outputFile.
 *
 * @param inputFile The name of the file to read from.
 * @param outputFile The name of the file to write to.
 */
void removeSpaces(const char *inputFile, const char *outputFile) {
    FILE *fin = fopen(inputFile, "r");
    FILE *fout = fopen(outputFile, "w");

    if (fin == NULL || fout == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    char ch;
    while ((ch = fgetc(fin)) != EOF) {
        if (ch != ' ') {
            fputc(ch, fout);
        }
    }

    fclose(fin);
    fclose(fout);
}

/**
 * Modifies the given string by adding spaces between each terminal and non-terminal
 * symbol. This is necessary because the grammar file is read in without spaces.
 *
 * @param str The string to modify.
 */
void addSpaces(char *str) {
    char result[MAX_LENGTH_PRODUCTION] = "";
    int len = strlen(str);

    for (int i = 0; i < len; i++) {
        if (str[i] == 'i' && i + 1 < len && str[i + 1] == 'd') {
            strcat(result, "id");
            i++;
            
            // Add space if not at the end
            if (i < len - 1) {
                strcat(result, " ");
            }
        } else {
            char temp[2] = {str[i], '\0'};
            strcat(result, temp);

            if (i < len - 1 && str[i] != ' ' && str[i + 1] != ' ') {
                strcat(result, " ");
            }
        }
    }

    strcpy(str, result);
}

char findAvailableNonTerminal() {
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (getIndex(c) == -1) {
            return c;
        }
    }
    // Handle error if no non-terminal is available
    printf("Error: No available non-terminals!\n");
    exit(1);
}

/**
 * Modifies the global grammar by adding spaces between each terminal and non-terminal
 * symbol in each production. This is necessary because the grammar file is read in
 * without spaces.
 */
void addSpacesToGrammar() {
    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < grammar[i].count; j++) {
            addSpaces(grammar[i].productions[j]);
        }
    }
}

/**
 * Copies the input string to the output string, removing all spaces.
 *
 * @param input The string to copy from.
 * @param output The string to copy to.
 */
void removeSpacesFromString(char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] != ' ') {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

/**
 * Modifies the global grammar by removing all spaces from each production. This
 * is necessary because the grammar file is read in with spaces.
 */
void removeSpaceFromGrammar() {
    for (int i = 0; i < numRules; i++) {
        for (int j = 0; j < grammar[i].count; j++) {
            char cleanedProduction[MAX_LENGTH_PRODUCTION];
            removeSpacesFromString(grammar[i].productions[j], cleanedProduction);
            strcpy(grammar[i].productions[j], cleanedProduction);
        }
    }
}

/**
 * Reads a grammar from a file and stores it in the global `grammar` array.
 *
 * This function reads the grammar file line by line, parsing each line as a rule
 * of the form "LHS -> RHS". It then breaks the RHS into individual productions
 * by splitting on the "|" character. The LHS and RHS are then stored in the
 * `grammar` array, which is updated to reflect the new rules added during the
 * parsing process.
 */
void readGrammarFromFile() {
    removeSpaces("grammar.txt", "build/output.txt");
    FILE *file = fopen("build/output.txt", "r");
    if (!file) {
        printf("Error opening file\n");
        exit(1);
    }

    char line[MAX_LENGTH_PRODUCTION * 2];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char *arrow = strstr(line, "->");
        if (!arrow) continue;

        *arrow = '\0';
        char *lhs = line;
        char *rhs = arrow + 2;

        while (*lhs == ' ') lhs++;
        while (*rhs == ' ') rhs++;

        if (strlen(lhs) == 0 || strlen(rhs) == 0) continue;

        grammar[numRules].nonTerminal = lhs[0];
        grammar[numRules].count = 0;

        char *token = strtok(rhs, "|");
        while (token) {
            while (*token == ' ') token++;
            strcpy(grammar[numRules].productions[grammar[numRules].count++], token);
            token = strtok(NULL, "|");
        }

        numRules++;
    }

    fclose(file);
}

/**
 * Prints the grammar rules stored in the global `grammar` array in a human-readable
 * format. The output is formatted as "LHS -> RHS1 | RHS2 | ...", where LHS is the
 * non-terminal and RHS1, RHS2, ... are the productions associated with that
 * non-terminal.
 */
void displayGrammar() {
    printf("Grammar Rules:\n");
    for (int i = 0; i < numRules; i++) {
        printf("%c -> ", grammar[i].nonTerminal);
        for (int j = 0; j < grammar[i].count; j++) {
            printf("%s", grammar[i].productions[j]);
            if (j < grammar[i].count - 1) printf(" | ");
        }
        printf("\n");
    }
}

/**
 * Checks if a given symbol is a terminal.
 *
 * A symbol is a terminal if it is not an uppercase letter (A-Z).
 *
 * @param symbol The symbol to check.
 * @return 1 if the symbol is a terminal, 0 otherwise.
 */
int isTerminal(char symbol) {
    return !(symbol >= 'A' && symbol <= 'Z');
}

/**
 * Finds the index of a given symbol in the global grammar array.
 *
 * @param symbol The symbol to search for.
 * @return The index of the symbol in the grammar array, or -1 if not found.
 */
int getIndex(char symbol) {
    for (int i = 0; i < numRules; i++) {
        if (grammar[i].nonTerminal == symbol) return i;
    }
    return -1;
}

/**
 * Adds a given symbol to a set, unless it is already present.
 *
 * @param set The set to add the symbol to.
 * @param symbol The symbol to add.
 * @param count The current count of elements in the set.
 */
void addToSet(char *set, char symbol, int *count) {
    for (int i = 0; i < *count; i++) {
        if (set[i] == symbol) return;
    }
    set[*count] = symbol;
    (*count)++;
}

/**
 * Checks if the given set contains epsilon.
 *
 * @param set The set of characters to check.
 * @param count The number of elements in the set.
 * @return 1 if epsilon is in the set, 0 otherwise.
 */
int containsEpsilon(char *set, int count) {
    for (int i = 0; i < count; i++) {
        if (set[i] == '#') return 1;
    }
    return 0;
}