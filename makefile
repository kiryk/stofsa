all: stofsa

clean:
	rm stofsa

stofsa: stofsa.c
	cc -Wall -ansi -o stofsa stofsa.c

.PHONY: all clean
