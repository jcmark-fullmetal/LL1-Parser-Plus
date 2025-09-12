#ifndef LL1_PARSER_H
#define LL1_PARSER_H

/* FIRST/FOLLOW computation and LL(1) parsing table */
void computeFirstSet(char nonTerminal);
void computeFollowSet(char nonTerminal);
void constructLL1ParsingTable();
void printFirstSets();
void printFollowSets();
void displayParsingTable();

#endif // LL1_PARSER_H