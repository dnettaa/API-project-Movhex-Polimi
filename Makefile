CC = gcc
CFLAGS = -Wall -Werror -std=gnu11 -O2
LDLIBS = -lm

TARGET = movhex
SRC = src/movhex.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(TARGET)
