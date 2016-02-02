# gallocy

<a href="https://travis-ci.org/sholsapp/gallocy">
<img src='https://img.shields.io/travis/sholsapp/gallocy/master.svg?style=flat-square' />
</a>

The gallocy library implements a memory allocator that transparently allocates
memory across many machines. Combined with a gallocy-aware implementation of
the pthreads library, this framework can make any pthreads application a
distributed program! The gallocy library exists entirely in
[userspace](http://www.linfo.org/user_space.html), so not kernel modifications
are necessary!

The gallocy library is *under active development* and is not ready for use. The
gallocy library is in a pre-alpha state, and probably will be for a very, very
long time. Reach out to us if you're interested in helping.

## requirements

You need to make sure your local environment has a few things before you can
get started. Here is a list of those things and our suggestions.

- cmake - *we use cmake to build our code*
- docker - *we use docker to run distributed tests*
- gcc/g++ 4.9+ - *we make thorough use of C++14*
- linux - *we strongly suggest using with a 3.16+ kernel*
- python - *we use python to build a distributed test framework*

You're probably already using most of these things if you're running on an
operating system released after 2015, but if not, setup one in a virtual
machine and remote in for your development work (hint: we develop on OS X and
remote into a Debian 8.2 (glibc 2.19) virtual machine running in VirtualBox).

This project uses [cthulhu](https://github.com/sholsapp/cthulhu) as a
distributed testing framework.

## getting started

Get the code using git.

```
git clone --recursive https://github.com/sholsapp/gallocy
```

Get started right away with the `project` helper.

```
Usage: project <subcommand> [options]
Subcommands:
    build - invoke cmake/make to build the code
    clean - clean things up
    integration - run the system test suite
    stylecheck - run the style checker
    test - run the unit test suite
```

Building is enough to get you started.

```bash
./project build
```

This will build Unix Makefiles, build library code, build sample application
code, build the google-test test driver, and then run the library code unit
tests. Output files will be placed into the `install` directory.

## documentation

  - [NUTSHELL](./resources/NUTSHELL.md) *good for learning about allocators*
  - [IMPLEMENTATION](./resources/IMPLEMENTATION.md) *good for learning about distributed shared memory*


## about

This project is the continuation of a [M.S. thesis and project from California
Polytechnic University, San Luis
Obispo](http://digitalcommons.calpoly.edu/theses/725/), that was just too much
fun to stop working on. *Substantial* improvements have since been made to the
original prototype that was presented in this thesis and now the major focuses
of this project are operability, maintainability, programability, and all the
other nice things that one learns from the software industry. It should be
*fun* to program on gallocy.

![Go Mustangs.](./resources/images/cal-poly.png)

If you're interested in using gallocy in your own project or extending gallocy
to support new features or coherency protocols, reach out to us to let us know!
We also have a ton of fun ideas for group projects, M.S. thesis projects, and
even Ph.D. dissertation areas in the fields of binary rewriting and live
process migration.
