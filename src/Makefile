CC = gcc
CFLAGS = -g
BUILD_DIR = build
SRC_DIR = .

# Source files
SRCS = main.c utils.c leftFactoring.c leftRecursion.c LL1Parser.c parsingStack.c stringParser.c

# Object files
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(BUILD_DIR) $(BUILD_DIR)/parser

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/parser: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"

run: all
	./$(BUILD_DIR)/parser input.txt

.PHONY: all clean run