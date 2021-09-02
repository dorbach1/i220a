PROJECT = quadr
SRC_FILES = main.c \
	    quadr.c \
	    test.data \
	    README 
CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT):	quadr.o main.o
		$(CC) $(LDFLAGS) quadr.o main.o -o $@

clean:
		rm -f *.o $(PROJECT) $(PROJECT).tar.gz
depend:
	$(CC) -MM $(CPPFLAGS) *.c

dist: $(PROJECT).tar.gz

$(PROJECT).tar.gz: $(SRC_FILES)
	tar -cf $(PROJECT).tar $(SRC_FILES)
	gzip -f $(PROJECT).tar


#Autimatically generated dependencied produced by make depend
main.o: main.c quadr.h
quadr.o: quadr.c quadr.h

