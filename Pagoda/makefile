#This is the makefile for the pagoda version.

CFLAGS = `sdl2-config --cflags` -O4 -Wall -pedantic -std=c99 -lm
INCS = neillsdl2.h pagoda_functions.h
TARGET = pagoda
SOURCES =  neillsdl2.c pagoda_functions.c $(TARGET).c
LIBS =  `sdl2-config --libs`
CC = gcc -g


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	$(TARGET) 