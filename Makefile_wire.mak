CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c avl_tree.c
OBJ = $(SRC:.c=.o)

all: bin/main

bin/main: $(OBJ)
    @mkdir -p bin
    $(CC) $(CFLAGS) -o bin/main $(OBJ)

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJ) bin/main
