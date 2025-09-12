#include "parsingStack.h"

/**
 * Initialize an empty stack.
 * 
 * @param s Pointer to the stack to initialize.
 */
void initStack(Stack *s) {
    s->top = -1;
}

/**
 * Check if the stack is empty.
 * 
 * @param s Pointer to the stack.
 * @return 1 if stack is empty, 0 otherwise.
 */
int isEmpty(Stack *s) {
    return s->top == -1;
}

/**
 * Push an item onto the stack.
 * 
 * @param s Pointer to the stack.
 * @param item The character to push onto the stack.
 */
void push(Stack *s, char item) {
    if (s->top == MAX_STACK_SIZE - 1) {
        printf("Stack overflow!\n");
        return;
    }
    // Add a space before pushing a new symbol (to separate symbols)
    if (s->top >= 0 && s->items[s->top] != ' ') {
        s->items[++(s->top)] = ' ';
    }
    s->items[++(s->top)] = item;
}

/**
 * Push a multi-character symbol onto the stack.
 * 
 * @param s Pointer to the stack.
 * @param symbol The string to push onto the stack.
 */
void pushSymbol(Stack *s, const char* symbol) {
    if (s->top == MAX_STACK_SIZE - 1) {
        printf("Stack overflow!\n");
        return;
    }
    
    // Add a space before pushing a new symbol (if stack is not empty)
    if (s->top >= 0 && s->items[s->top] != ' ') {
        s->items[++(s->top)] = ' ';
    }
    
    // Push the symbol characters in reverse order
    int len = strlen(symbol);
    for (int i = len - 1; i >= 0; i--) {
        s->items[++(s->top)] = symbol[i];
    }
}

/**
 * Pop a symbol from the stack.
 * 
 * @param s Pointer to the stack.
 * @param buffer Buffer to store the popped symbol.
 * @return 1 if successful, 0 if stack is empty.
 */
int popSymbol(Stack *s, char* buffer) {
    if (isEmpty(s)) {
        printf("Stack underflow!\n");
        buffer[0] = '\0';
        return 0;
    }
    
    int i = 0;
    // Pop characters until we hit a space or stack becomes empty
    while (!isEmpty(s) && s->items[s->top] != ' ') {
        buffer[i++] = s->items[(s->top)--];
    }
    
    // Pop the space separator if present
    if (!isEmpty(s) && s->items[s->top] == ' ') {
        s->top--;
    }
    
    // Null-terminate the buffer
    buffer[i] = '\0';
    
    // Reverse the buffer since we popped characters in reverse order
    int left = 0, right = i - 1;
    while (left < right) {
        char temp = buffer[left];
        buffer[left] = buffer[right];
        buffer[right] = temp;
        left++;
        right--;
    }
    
    return 1;
}

/**
 * Pop an item from the stack.
 * 
 * @param s Pointer to the stack.
 * @return The character popped from the stack, or '\0' if stack is empty.
 */
char pop(Stack *s) {
    char symbol[MAX_LENGTH];
    if (popSymbol(s, symbol)) {
        return symbol[0]; // Return only the first character for backward compatibility
    }
    return '\0';
}

/**
 * Peek at the top symbol in the stack without removing it.
 * 
 * @param s Pointer to the stack.
 * @param buffer Buffer to store the peeked symbol.
 * @return 1 if successful, 0 if stack is empty.
 */
int peekSymbol(Stack *s, char* buffer) {
    if (isEmpty(s)) {
        buffer[0] = '\0';
        return 0;
    }
    
    int i = 0;
    int pos = s->top;
    
    // Read characters until we hit a space or reach the bottom of the stack
    while (pos >= 0 && s->items[pos] != ' ') {
        buffer[i++] = s->items[pos--];
    }
    
    // Null-terminate the buffer
    buffer[i] = '\0';
    
    // Reverse the buffer since we read characters in reverse order
    int left = 0, right = i - 1;
    while (left < right) {
        char temp = buffer[left];
        buffer[left] = buffer[right];
        buffer[right] = temp;
        left++;
        right--;
    }
    
    return 1;
}

/**
 * Peek at the top item in the stack without removing it.
 * 
 * @param s Pointer to the stack.
 * @return The character at the top of the stack, or '\0' if stack is empty.
 */
char peek(Stack *s) {
    char symbol[MAX_LENGTH];
    if (peekSymbol(s, symbol)) {
        return symbol[0]; // Return only the first character for backward compatibility
    }
    return '\0';
}

/**
 * Display the contents of the stack.
 * 
 * @param s Pointer to the stack.
 */
void displayStack(Stack *s) {
    printf("Stack: ");
    if (isEmpty(s)) {
        printf("empty");
    } else {
        // We'll read the stack directly for display purposes
        int i = 0;
        while (i <= s->top) {
            if (s->items[i] == ' ') {
                printf(" "); // Print spaces as separators
            } else {
                printf("%c", s->items[i]);
            }
            i++;
        }
    }
    printf("\n");
}