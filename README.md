# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://secure.travis-ci.org/sholsapp/gallocy.png?branch=master'>
</a>

> I want to a run program that uses ``1 exabyte`` of memory.

The gallocy library is a custom memory allocator that enables processes to
utilize memory on many machines. One could call gallocy a distributed shared
memory allocator.

  - [DESIGN](./DESIGN.md)
  - [IMPLEMENTATION](./IMPLEMENTATION.md)

## getting started

To get started right away, run the `build` helper script.

```bash
build
```

### building

We use cmake to build gallocy. Use the following commands on a Unix-style host
to generate the makefiles.

```bash
cmake -G "Unix Makefiles"
make && make install
```

This installs test binaries and libraries into the build/ directory.

### testing

We use gtest to test gallocy. Use the following commands on a Unix-style host
to run the test binary.

```bash
export DYLD_LIBRARY_PATH=`pwd`/build/lib
export LD_LIBRARY_PATH=`pwd`/build/lib
./build/bin/sample_tests
```
