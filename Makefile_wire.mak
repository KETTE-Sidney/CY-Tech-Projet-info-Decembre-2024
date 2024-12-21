# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target
TARGET = main

.PHONY: all clean run

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean build files
clean:
	rm -f $(TARGET)

# Run the program
run: all
	chmod +x c-wire.sh
	./c-wire.sh input.csv hvb comp

