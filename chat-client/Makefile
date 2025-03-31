CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinc/
LDFLAGS = -lncurses

# Source and object files
SRC = src/main.c src/client.c src/ui.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = chat-client

# Default build target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile .c files into .o objects
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean  # Declare phony targets