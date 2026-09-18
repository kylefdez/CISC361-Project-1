CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -g
TARGET  = slist
OBJECTS = main.o slist.o
HEADERS = slist.h

.PHONY: all clean memcheck

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

slist.o: slist.c $(HEADERS)
	$(CC) $(CFLAGS) -c slist.c

clean:
	rm -f $(OBJECTS) $(TARGET)

memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)
