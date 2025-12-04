CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread -I./include
TARGET = main
CHILD1 = child1
CHILD2 = child2

all: $(TARGET) $(CHILD1) $(CHILD2)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c -lrt

$(CHILD1): src/child1.c src/string_to_uppercase.c
	$(CC) $(CFLAGS) -o $(CHILD1) src/child1.c src/string_to_uppercase.c -lrt

$(CHILD2): src/child2.c src/del_space.c
	$(CC) $(CFLAGS) -o $(CHILD2) src/child2.c src/del_space.c -lrt

clean:
	rm -f $(TARGET) $(CHILD1) $(CHILD2) *.o

run: all
	./$(TARGET)

.PHONY: all clean run