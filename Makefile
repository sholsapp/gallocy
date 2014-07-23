CPP=g++
CFLAGS= -Wall -Isrc -Isrc/externals/sqlite3 -g
LDFLAGS= -shared -fPIC -lpthread -ldl

BUILD_DIR=$(shell pwd)

TESTS=simple simple-free nested-malloc stlallocator-test double-ptr-test
EXTERNAL=externals
ALL=gallocy $(EXTERNALS) $(TESTS)
all:	$(ALL)

# This is the main shared library

gallocy:
	$(CPP) $(CFLAGS) -o libgallocy.so $(LDFLAGS) src/libgallocy.cpp src/wrapper.cpp

externals:
	gcc $(CFLAGS) -o sqlite3.so $(LDFLAGS) src/external/sqlite3/shell.c src/external/sqlite3/sqlite3.c

# Put tests below this comment

simple: gallocy
	$(CPP) $(CFLAGS) test/simple.c -o simple-test -L. -lgallocy

simple-free: gallocy
	$(CPP) $(CFLAGS) test/simple-free.c -o simple-free-test -L. -lgallocy

nested-malloc: gallocy
	$(CPP) $(CFLAGS) test/nested-malloc.c -o nested-malloc-test -L. -lgallocy

stlallocator-test: gallocy
	$(CPP) $(CFLAGS) test/stlallocator-test.c -o stlallocator-test -L. -lgallocy

double-ptr-test: gallocy
	$(CPP) $(CFLAGS) test/double-ptr.c -o double-ptr-test -L. -lgallocy

clean:
	rm -rf libgallocy.so *-test *.dSYM
