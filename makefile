CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
LDFLAGS = 

SRC = disk_monitor.c
OBJ = $(SRC:.c=.o)
TARGET = disk

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

