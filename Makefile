CPP=g++
CFLAGS= -Wall -Isrc -Isrc/external/sqlite3 -g -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer
LDFLAGS= -shared -fPIC -lpthread -ldl

BUILD_DIR=$(shell pwd)

TESTS=simple-malloc-test simple-free-test stlallocator-test mmult-test sqlite-test
EXTERNAL=externals
ALL=gallocy $(EXTERNALS) $(TESTS)
all:	$(ALL)

# This is the main shared library

gallocy: sqlite3.o
	$(CPP) $(CFLAGS) -o libgallocy.so sqlite3.o $(LDFLAGS) src/libgallocy.cpp src/wrapper.cpp src/pagetable.cpp

sqlite3.o:
	gcc $(CLFAGS) -c -o sqlite3.o src/external/sqlite3/sqlite3.c -fPIC

externals:
	gcc $(CFLAGS) -o sqlite3.so $(LDFLAGS) src/external/sqlite3/shell.c src/external/sqlite3/sqlite3.c

# Put tests below this comment

simple-malloc-test: gallocy
	$(CPP) $(CFLAGS) test/simple.c -o simple-malloc-test -L. -lgallocy

simple-free-test: gallocy
	$(CPP) $(CFLAGS) test/simple-free.c -o simple-free-test -L. -lgallocy

stlallocator-test: gallocy
	$(CPP) $(CFLAGS) test/stlallocator-test.c -o stlallocator-test -L. -lgallocy

mmult-test: gallocy
	$(CPP) $(CFLAGS) test/mmult.c -o mmult-test -L. -lgallocy

sqlite.o:
	gcc $(CFLAGS) -Isrc/external/sqlite3 src/external/sqlite3/sqlite3.c -c -o sqlite.o

sqlite-test: sqlite.o
	$(CPP) $(CFLAGS) -Isrc/external/sqlite3 sqlite.o test/sqlite.c -o sqlite-test -lpthread -ldl

clean:
	rm -rf libgallocy.so *-test *.dSYM test.db *.o
