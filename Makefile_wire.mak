# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = projetwire
SHELL_SCRIPT = c-wire.sh

# Cibles principales
all: $(TARGET)

$(TARGET): projetwire.c
	$(CC) $(CFLAGS) -o $(TARGET) projetwire.c

# Nettoyage
clean:
	rm -f $(TARGET)

# Exécution
run: $(TARGET)
	./$(TARGET)

script:
	chmod +x $(SHELL_SCRIPT)
	./$(SHELL_SCRIPT)
