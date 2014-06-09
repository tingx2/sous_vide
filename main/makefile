#######################################################################
#
# Makefile
#
#######################################################################

# Use gcc for C program compilation
CC = gcc

# Compiler flags
# -Wall : turns on all compiler warnings
CFLAGS = -Wall -lrt

# Target executable
TARGET = cook 

all: $(TARGET)

$(TARGET): main.o timer.o temp.o pwm.o debug.o pid.o
	$(CC) $(CFLAGS) main.o timer.o debug.o temp.o pwm.o pid.o -lbcm2835 -o $(TARGET)

main.o: main.c types.h timer.h debug.h pid.h
	$(CC) $(CFLAGS) -c main.c

timer.o: timer.c types.h timer.h
	$(CC) $(CFLAGS) -c timer.c

temp.o: temp.c temp.h
	$(CC) $(CFLAGS) -c temp.c

pwm.o: pwm.c pwm.h timer.h
	$(CC) $(CFLAGS) -c pwm.c

debug.o: debug.c types.h debug.h
	$(CC) $(CFAGS) -c debug.c

pid.o: pid.c types.h pid.h
	$(CC) $(CFAGS) -c pid.c

clean:
	rm -rf *o $(TARGET)
