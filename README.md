# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://secure.travis-ci.org/sholsapp/gallocy.png?branch=master'>
</a>

The gallocy library implements a memory allocator that transparently allocates
memory across n-many machines, effectively making any program a distributed
program.

![This is Exabyte.](./resources/images/exabyte-logo.png)

>  I'm a distributed shared memory allocator!

  - [DESIGN](./resources/DESIGN.md)
  - [IMPLEMENTATION](./resources/IMPLEMENTATION.md)

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
