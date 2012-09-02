CPP=g++
CFLAGS= -Wall -Isrc
LDFLAGS= -shared

BUILD_DIR=$(shell pwd)

TESTS=simple nested-malloc stlallocator-test
ALL=gallocy $(TESTS)
all:	$(ALL)

# This is the main shared library

gallocy:
	$(CPP) $(CFLAGS) -o libgallocy.so $(LDFLAGS) src/libgallocy.cpp src/wrapper.cpp

# Put tests below this comment

simple: gallocy
	$(CPP) $(CFLAGS) test/simple.c -o simple-test -L. -lgallocy

nested-malloc: gallocy
	$(CPP) $(CFLAGS) test/nested-malloc.c -o nested-malloc-test -L. -lgallocy

stlallocator-test: gallocy
	$(CPP) $(CFLAGS) test/stlallocator-test.c -o stlallocator-test -L. -lgallocy

clean:
	rm -rf libgallocy.so *-test
