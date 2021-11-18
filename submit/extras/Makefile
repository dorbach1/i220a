CFLAGS = -g -Wall -std=c18  -fPIC

TARGETS = fns fnsTest
all:		$(TARGETS)

#Target used to build shared object module containing functions used
#for testing.  Presupposes existence of fns.c (which could have been
#generated automatically).
fns:		fns.c
		$(CC) $(CFLAGS) -shared $< -o $@

#Target used to build fns.c into a standalone program using 
#conditionally-included TEST_TESTFN main().
fnsTest:	fns.c
		$(CC) -DTEST_TESTFN $(CFLAGS) $< -o $@

clean:
		rm -f *~ $(TARGETS)
