
CC=g++
CFLAGS=-g -Wall -Wextra
LIBS=-lpthread

SOURCES=$(wildcard ./*.cc)
OBJECTS=$(patsubst %.cc, %.o, $(SOURCES))

TARGET=test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

objs/%.o: %.c
	$(CC) -c -o $@ $<


clean:
	rm $(TARGET) *.o
