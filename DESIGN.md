# gallocy

## design

Building a distributed shared memory system is easy. This section goes over the
major design blocks needed to understand how such systems work.

### definitions

_the game_ | An application running with gallocy as the memory allocator.

### memory

There are fundamentally three different regions of memory that a dsm needs to
consider: i) a page table, ii) internal memory, iii) application memory.

#### page table

The page table must be maintained so that the properties of the application
memory can be maintained. This page table can tell us things like what memory
is currently mapped in the application, who owns this memory, or what the local
permissions on that memory block currently are.

This page table must be maintained in a dedicated memory region so that it may
easily synchronized between participants in the game.

#### internal memory

The internal memory region can also be referred to as local memory. This region
of memory is a good place to put local threads, process-specific data, and
other things of that nature.

This is the only region of memory that is allowed to differ between
participants in the game.

#### application memory

The application memory is the region of memory that is managed by the page
table and actually used by the application. Sometimes, I refer to the
application as the game to make it easier to distinguish between the library
that is running the dsm stack.

This region of memory must follow whatever coherency paradigm we implement in
gallocy, be it strong consistency, eventual consistency, lazy consistency, or,
lazy-release consistency.

### allocators

We use a design paradigm called _Mixins_ -- see
http://people.cs.umass.edu/~yannis/practical-fmtd.pdf for more detals.

Each allocator mixin is required to implement the following methods:

- malloc
- free
- getSize
- __reset
