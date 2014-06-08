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

$(TARGET): main.o timer.o debug.o pid.o
	$(CC) $(CFLAGS) main.o timer.o debug.o pid.o -o $(TARGET)

main.o: main.c types.h timer.h debug.h pid.h
	$(CC) $(CFLAGS) -c main.c

timer.o: timer.c types.h timer.h
	$(CC) $(CFLAGS) -c timer.c

debug.o: debug.c types.h debug.h
	$(CC) $(CFAGS) -c debug.c

pid.o: pid.c types.h pid.h
	$(CC) $(CFAGS) -c pid.c

clean:
	rm -rf *o $(TARGET)
