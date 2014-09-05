# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://secure.travis-ci.org/sholsapp/gallocy.png?branch=master'>
</a>

The gallocy library is a custom memory allocator that enables processes to
utilize memory on many machines. One could call gallocy a distributed shared
memory allocator.

It borrows a large amount of design philosphy from the Hoard
(http://www.hoard.org/) memory allocator, and was originally prototyped as a
thesis. The gallocy project is a complete rewrite of that prototype, that seeks
to apply the many lessons and shortcomings discovered during those miserable
few years.

## getting started

### building

We use cmake to build gallocy. Use the following commands on a Unix-style host
to generate the makefiles.

```bash
cmake -G "Unix Makefiles"
make && make install
```

This installs test binaries and libraryes into the build/ directory.

### testing

We use gtest to test gallocy. Use the following commands on a Unix-style host
to run the test binary.

```bash
export DYLD_LIBRARY_PATH=`pwd`/build/lib
export LD_LIBRARY_PATH=`pwd`/build/lib
./build/bin/sample_tests
```
