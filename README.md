# LL(1) Parser with String Parsing and Stack Visualization

C project that reads a grammar, performs left factoring and left recursion removal, computes FIRST/FOLLOW sets, constructs an LL(1) parsing table, and parses input strings using a custom stack implementation. Includes sample grammar and input, Makefile, and Windows PowerShell build script.

## Features
- Read grammar from `src/grammar.txt`
- Left factoring and left recursion elimination
- Compute and print FIRST and FOLLOW sets
- Build and display LL(1) parsing table
- Parse multiple input strings from `src/input.txt`
- Custom stack with multi-character token handling and debug prints

## Project Structure
```
./
  src/
    grammar.txt
    input.txt
    leftFactoring.c/.h
    leftRecursion.c/.h
    LL1Parser.c/.h
    parsingStack.c/.h
    stringParser.c/.h
    utils.c/.h
    main.c
    Makefile
  README.md
```

## Build
Using the Makefile (recommended):
```
cd src
make
```
Produces `build/parser`.

## Run
```
cd src
make run
```
This runs `build/parser src/input.txt` (Makefile uses `input.txt`).

## Clean
```
cd src
make clean
```

## Windows (PowerShell) alternative
If `make` isn’t available, a simple script can be used:
```
cd src
pwsh -NoProfile -Command "mkdir build -ErrorAction SilentlyContinue; gcc -g -c main.c utils.c leftFactoring.c leftRecursion.c LL1Parser.c parsingStack.c stringParser.c; gcc -g -o build/parser main.o utils.o leftFactoring.o leftRecursion.o LL1Parser.o parsingStack.o stringParser.o; ./build/parser input.txt"
```

## Notes
- The parser expects terminals like `id` (two characters) and handles them consistently in the stack and table.
- `utils.c` writes a space-stripped copy of the grammar to `build/output.txt` during processing.
