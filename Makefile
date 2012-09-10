CPP=g++
CFLAGS= -Wall -Isrc -g
LDFLAGS= -shared

BUILD_DIR=$(shell pwd)

TESTS=simple simple-free nested-malloc stlallocator-test double-ptr-test
ALL=gallocy $(TESTS)
all:	$(ALL)

# This is the main shared library

gallocy:
	$(CPP) $(CFLAGS) -o libgallocy.so $(LDFLAGS) src/libgallocy.cpp src/wrapper.cpp

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
	rm -rf libgallocy.so *-test
