# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://secure.travis-ci.org/sholsapp/gallocy.png?branch=master'>
</a>

The gallocy library implements a memory allocator that transparently allocates
memory across n-many machines, effectively making any program a distributed
program. The gallocy library exists entirely in
[userspace](http://www.linfo.org/user_space.html), so not kernel modifications
are necessary!

![This is Exabyte.](./resources/images/exabyte-logo.png)

>  I'm a distributed shared memory allocator!

The gallocy library is *under active development* and is not ready for use. The
gallocy library is in a pre-alpha state, and probably will be for a very, very
long time.

## documentation

  - [NUTSHELL](./resources/NUTSHELL.md)
  - [DESIGN](./resources/DESIGN.md)
  - [IMPLEMENTATION](./resources/IMPLEMENTATION.md)

## getting started

To get started right away, run the `build` helper script.

```bash
build
```

This will build Unix make files, build library code, build google-test test
driver, and then run the library code unit tests.

### building

We use cmake to build gallocy. Use the following commands on a Unix-style host
to generate the makefiles.

```bash
cmake -G "Unix Makefiles"
make && make install
```

This installs test binaries and libraries into the `install/bin` and
`install/lib` directories, respectively.

### testing

We use gtest to test gallocy. Use the following commands on a Unix-style host
to run the test binary.

```bash
export DYLD_LIBRARY_PATH=`pwd`/build/lib
export LD_LIBRARY_PATH=`pwd`/build/lib
./build/bin/sample_tests
```
