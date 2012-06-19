CPP=g++

ALL=gallocy
all:	$(ALL)

gallocy:
	g++ -Wall -Isrc -o libgallocy.so -shared src/libgallocy.cpp src/wrapper.cpp

clean:
	rm -rf libgallocy.so
