#ifndef PARSING_STACK_H
#define PARSING_STACK_H

/* Simple stack with spacing, supports multi-character symbols for display */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 100
#define MAX_LENGTH 100

typedef struct {
    char items[MAX_STACK_SIZE];
    int top;
} Stack;

void initStack(Stack *s);
int isEmpty(Stack *s);
void push(Stack *s, char item);
char pop(Stack *s);
char peek(Stack *s);
void displayStack(Stack *s);

// New functions for multi-character symbols
void pushSymbol(Stack *s, const char* symbol);
int popSymbol(Stack *s, char* buffer);
int peekSymbol(Stack *s, char* buffer);

#endif