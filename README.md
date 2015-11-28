# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://img.shields.io/travis/sholsapp/gallocy/master.svg?style=flat-square' />
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

  - [NUTSHELL](./resources/NUTSHELL.md) *good for getting started with allocators*
  - [DESIGN](./resources/DESIGN.md) *good if you're still curious*
  - [IMPLEMENTATION](./resources/IMPLEMENTATION.md) *good for the makers*

## getting started

Get started right away with the `project` helper.

```
Usage: project <subcommand> [options]
Subcommands:
    build - invoke cmake/make to build the code
    clean - clean things up
    fuckit - seriously, clean everything up
    integration - run the system test suite
    stylecheck - run the style checker
    test - run the unit test suite
```

Building is enough to get you started.

```bash
./project build
```

This will build Unix make files, build library code, build google-test test
driver, and then run the library code unit tests.
