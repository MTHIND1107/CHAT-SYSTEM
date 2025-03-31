CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinc/

# Source and object files
SRC = src/server.c src/client_handler.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = chat-server

# Default build target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files into .o objects
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean  # Declare phony targets