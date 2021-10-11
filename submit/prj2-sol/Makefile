PROJECT = int-set
CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT): int-set.o int-set-strings.o main.o 
	$(CC) int-set.o int-set-strings.o main.o $(LDFLAGS) -o $@

depend: 
	$(CC) -MM $(CFLAGS) *.c



int-set.o: int-set.c int-set.h
int-set-strings.o: int-set-strings.c int-set.h int-set-strings.h

	

