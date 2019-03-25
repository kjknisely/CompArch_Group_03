# Makefile for computer architecture group project
# Richard Azille
# Keegan Knisely
# Sabrina Mosher

PROGRAMS = \
	Sim.exe

CC = gcc
CFLAGS = -g -Wall -lm

all: $(PROGRAMS)

tidy:
	rm -f ,* .,* *~ core a.out graphics.ps
scratch:
	rm -f *.o *.a $(PROGRAMS)

Sim.exe: Sim.c
	$(CC) $(CFLAGS) Sim.c -o Sim.exe

