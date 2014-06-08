#######################################################################
#
# Makefile
#
#######################################################################

# Use gcc for C program compilation
CC = gcc

# Compiler flags
# -Wall : turns on all compiler warnings
CFLAGS = -Wall

# Target executable
TARGET = cook 

all: $(TARGET)

$(TARGET): main.o timer.o
	$(CC) $(CFLAGS) main.o timer.o -o $(TARGET)

main.o: main.c types.h timer.h
	$(CC) $(CFLAGS) -c main.c

timer.o: timer.c types.h
	$(CC) $(CFLAGS) -c timer.c

clean:
	rm -rf *o $(TARGET)

