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

  - [NUTSHELL](./resources/NUTSHELL.md) *good for getting started with allocators*
  - [DESIGN](./resources/DESIGN.md) *good if you're still curious*
  - [IMPLEMENTATION](./resources/IMPLEMENTATION.md) *good for the makers*

## getting started

Get started run away with the `project` helper.

```bash
./project build
```

This will build Unix make files, build library code, build google-test test
driver, and then run the library code unit tests.
