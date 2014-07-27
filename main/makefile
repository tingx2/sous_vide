#######################################################################
#
# Makefile
#
#######################################################################

# Use gcc for C program compilation
CC = gcc

# Compiler flags
# -Wall : turns on all compiler warnings
CFLAGS = -Wall -std=c99

# Target executable
TARGET = cook 

all: $(TARGET)

$(TARGET): main.o timer.o debug.o pid.o pwm.o therm.o
	$(CC) $(CFLAGS) main.o timer.o debug.o pid.o pwm.o therm.o -lbcm2835 -lrt -o $(TARGET)

main.o: main.c types.h timer.h debug.h pid.h pwm.h therm.h
	$(CC) $(CFLAGS) -c main.c

timer.o: timer.c timer.h types.h debug.h
	$(CC) $(CFLAGS) -c timer.c

debug.o: debug.c debug.h types.h timer.h
	$(CC) $(CFLAGS) -c debug.c

pid.o: pid.c pid.h types.h debug.h timer.h pwm.h
	$(CC) $(CFLAGS) -c pid.c

pwm.o: pwm.c pwm.h types.h debug.h timer.h
	$(CC) $(CFLAGS) -c pwm.c

therm.o: therm.c therm.h types.h debug.h timer.h
	$(CC) $(CFLAGS) -c therm.c


clean:
	rm -rf *o $(TARGET)
