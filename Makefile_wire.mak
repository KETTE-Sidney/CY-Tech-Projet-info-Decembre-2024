# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g

# Targets
all: main

# Linking the executable
main: projetwire.o
        $(CC) $(CFLAGS) -o main projetwire.o

# Compiling the main file
main.o: projetwire.c
        $(CC) $(CFLAGS) -c projetwire.c


# Cleaning up generated files
clean:
        rm -f *.o main