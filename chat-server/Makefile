CC = gcc
CFLAGS = -Wall -Wextra -pthread
SRC = src/server.c src/client_handler.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = chat-server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)