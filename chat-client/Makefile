CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lncurses
SRC = src/client.c src/ui.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = chat-client

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)