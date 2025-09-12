#include "stringParser.h"
#include "LL1Parser.h"
#include "utils.h"

/**
 * Parse a single input string using the LL(1) parsing table.
 * 
 * @param inputString The string to parse.
 * @param lineErrors Pointer to store detailed error information.
 * @return 0 on successful parse, number of errors encountered otherwise.
 */
int parseString(char* inputString, LineErrors* lineErrors) {
    Stack stack;
    initStack(&stack);
    
    // Initialize the stack with $ and the start symbol
    push(&stack, '$');
    push(&stack, grammar[0].nonTerminal);
    
    int errorCount = 0;
    int i = 0;
    char currentSymbol[MAX_LENGTH];
    
    printf("Starting parsing...\n");
    printf("Stack: $ %c\n", grammar[0].nonTerminal);
    
    while (!isEmpty(&stack) && errorCount < 20) { // Limit errors to prevent infinite loops
        // Skip whitespace
        while (inputString[i] == ' ') i++;
        
        // Handle multi-character terminals
        if (inputString[i] == 'i' && inputString[i+1] == 'd') {
            strcpy(currentSymbol, "id");
            printf("Current input: id\n");
        } else if (inputString[i] == '\0') {
            strcpy(currentSymbol, "$"); // End of input
            printf("Current input: $\n");
        } else {
            // Single character terminal
            currentSymbol[0] = inputString[i];
            currentSymbol[1] = '\0';
            printf("Current input: %c\n", currentSymbol[0]);
        }
        
        char stackTop = peek(&stack);
        
        // Terminal matching
        if (isTerminal(stackTop)) {
            if (stackTop == '#') {
                // Handle epsilon - simply pop it
                pop(&stack);
                printf("Apply epsilon production\n");
            } else if ((stackTop == 'i' && strcmp(currentSymbol, "id") == 0) || 
                      (stackTop != 'i' && stackTop == currentSymbol[0])) {
                printf("Match: %s\n", currentSymbol);
                pop(&stack); // Discard matched symbol
                
                // For 'id', we need to pop one more character
                if (stackTop == 'i') {
                    if (peek(&stack) == 'd') {
                        pop(&stack);
                    } else {
                        // This shouldn't happen with correct parsing
                        printf("Error: Expected 'd' after 'i' on stack\n");
                        errorCount++;
                    }
                }
                
                // Move forward in the input string
                i += strlen(currentSymbol);
            } else {
                // Error recovery: report error and skip the expected symbol
                if (lineErrors->count < 10) {
                    if (stackTop == 'i' && peek(&stack) == 'd') {
                        snprintf(lineErrors->messages[lineErrors->count++], 100, 
                                "Expected 'id', found '%s'", currentSymbol);
                        printf("Error: Expected 'id', found '%s'\n", currentSymbol);
                        pop(&stack); // Pop 'i'
                        pop(&stack); // Pop 'd'
                    } else {
                        snprintf(lineErrors->messages[lineErrors->count++], 100, 
                                "Expected '%c', found '%s'", stackTop, currentSymbol);
                        printf("Error: Expected '%c', found '%s'\n", stackTop, currentSymbol);
                        pop(&stack); // Pop the expected terminal
                    }
                }
                
                errorCount++;
            }
        }
        // Non-terminal processing
        else {
            int nonTermIdx = getIndex(stackTop);
            
            // Use unsigned char for array index when looking up in parsing table
            unsigned char idx;
            if (strcmp(currentSymbol, "id") == 0) {
                idx = (unsigned char)'i';  // Use 'i' to represent 'id' in the parsing table
            } else {
                idx = (unsigned char)currentSymbol[0];
            }
            
            if (nonTermIdx != -1 && parsingTable[nonTermIdx][idx][0] != '\0') {
                char* production = parsingTable[nonTermIdx][idx];
                printf("Apply production: %c -> %s\n", stackTop, production);
                
                // Pop the non-terminal
                pop(&stack);
                
                // Skip pushing epsilon
                if (strcmp(production, "#") != 0) {
                    // Push the production in reverse order
                    for (int j = strlen(production) - 1; j >= 0; j--) {
                        // Special handling for 'id'
                        if (j > 0 && production[j-1] == 'i' && production[j] == 'd') {
                            push(&stack, 'd');
                            push(&stack, 'i');
                            j--; // Skip the 'i' since we've already processed it
                        } else {
                            push(&stack, production[j]);
                        }
                    }
                }
            } else {
                // Error recovery: report error and skip the non-terminal
                if (lineErrors->count < 10) {
                    snprintf(lineErrors->messages[lineErrors->count++], 100, 
                             "No production for %c with input %s", stackTop, currentSymbol);
                }
                printf("Error: No production for %c with input %s\n", stackTop, currentSymbol);
                
                // Error recovery: skip this non-terminal and continue
                pop(&stack);
                errorCount++;
            }
        }
        
        printf("Stack: ");
        if (isEmpty(&stack)) {
            printf("empty");
        } else {
            Stack tempStack;
            initStack(&tempStack);
            char symbol;
            
            while (!isEmpty(&stack)) {
                symbol = pop(&stack);
                push(&tempStack, symbol);
            }
            
            while (!isEmpty(&tempStack)) {
                symbol = pop(&tempStack);
                printf("%c ", symbol);
                push(&stack, symbol);
            }
        }
        printf("\n\n");
    }
    
    // Check if we consumed all input
    while (inputString[i] == ' ') i++;
    if (inputString[i] != '\0') {
        if (lineErrors->count < 10) {
            snprintf(lineErrors->messages[lineErrors->count++], 100, 
                     "Unexpected input after parsing completed: %s", &inputString[i]);
        }
        printf("Error: Unexpected input after parsing completed: %s\n", &inputString[i]);
        errorCount++;
    }
    
    return errorCount;
}

/**
 * Parse an input file containing strings to be parsed line by line.
 * 
 * @param filename The name of the file to parse.
 */
void parseInputFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening input file: %s\n", filename);
        exit(1);
    }
    
    char line[1000];
    int lineNumber = 1;
    int totalErrors = 0;
    
    // Array to store detailed error information for each line
    LineErrors allErrors[100] = {0}; // Up to 100 lines
    
    printf("\n=== Parsing Input Strings ===\n\n");
    
    while (fgets(line, sizeof(line), file) && lineNumber <= 100) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
        
        printf("Line %d: \"%s\"\n", lineNumber, line);
        printf("--------------------------------\n");
        
        // Initialize the line errors
        allErrors[lineNumber-1].lineNumber = lineNumber;
        allErrors[lineNumber-1].count = 0;
        
        int errors = parseString(line, &allErrors[lineNumber-1]);
        
        if (errors == 0) {
            printf("Line %d: Parsing successful.\n", lineNumber);
        } else {
            printf("Line %d: Parsing completed with %d error(s).\n", lineNumber, errors);
            totalErrors += errors;
        }
        
        printf("--------------------------------\n\n");
        lineNumber++;
    }
    
    fclose(file);
    
    // Print summary in the format shown in the assignment example
    printf("============== Parsing Summary ==============\n");
    
    for (int i = 0; i < lineNumber-1; i++) {
        if (allErrors[i].count > 0) {
            printf("Line %d: Syntax Error: %d errors detected\n", i+1, allErrors[i].count);
            
            // Print detailed error messages for this line
            for (int j = 0; j < allErrors[i].count; j++) {
                printf("  - %s\n", allErrors[i].messages[j]);
            }
            printf("\n");
        } else {
            printf("Line %d: Parsing successful.\n", i+1);
        }
    }
    
    if (totalErrors == 0) {
        printf("All strings parsed successfully.\n");
    } else {
        printf("Parsing completed with a total of %d error(s).\n", totalErrors);
    }
}