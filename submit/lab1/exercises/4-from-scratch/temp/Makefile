PROJECT = key-value
SRC_FILES = main.c \
	    process-file.c \
	    process-file.h \
	    key-value.c \
	    key-value.h \
	    README \
	    Makefile


CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT): key-value.o process-file.o main.o
	$(CC) key-value.o process-file.o main.o $(LDFLAGS) -o $@

clean: 
	rm -f *.o $(PROJECT) $(PROJECT).tar.gz

depend:
	$(CC) -MM $(CPPFLAGS) *.c

dist: $(PROJECT).tar.gz

$(PROJECT).tar.gz: $(SRC_FILES)
	tar -cf $(PROJECT).tar $(SRC_FILES)
	gzip -f $(PROJECT).tar

#Dependencies generated by MakeDepend
main.o: main.c process-file.h
process-file.o: process-file.c key-value.h process-file.h
