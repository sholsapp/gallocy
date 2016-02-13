# gallocy

Building a distributed shared memory infrastructure is *easy*.

## design

At a high level, the things a distributed shared memory infrastructure
architect cares about are:

  - state - *what are the major stateful objects*
  - memory - *what is memory and how does it behave*
  - consensus - *how does the distributed system coordinate change*
  - interface - *what does the interface to the application look like*

### state

The single most important object involved in operating a distributed shared
memory infrastructure is the *virtual memory manager* (VMM). This name was
respectfully stolen from the context of operating systems because of its
obvious similarity. Just as an operating system's VMM manages memory on a
single computer, a distributed shared memory infrastructure VMM  manages memory
accross many computers.

At its heart a VMM manages a data structure called the *page table*. A page is
often used to describe a region of memory and is often the most granluar unit
of address to the system, often 4096 bytes. The page table is a mapping between
a region of memory and metadata.  The metadata likely contains such data as who
can read or write to the region of memory. A distributed shared memory
infrastructure is free to track whatever metadata that it wishes, but beware,
tracking too much metadata will cause the page table to grow prohibitevly large
as the heap grows.

The VMM is the brain that powers system calls like `mmap` or `sbrk`, which are
invoked by standard library functions like `malloc` or `free`. Uses of these
system calls interact with the VMM and cause the VMM to update its page table
in some way.

It is crucial to understand that the distributed shared memory infrastructure's
VMM does not *replace* the operating system's VMM: it enhances it with higher
level logic.  For example, a distributed shared memory infrastructure, upon a
call to `mmap` or `sbrk`, would invoke a consensus module to notify others that
memory has been mapped into the address space, which in turn relies on each of
the other nodes' operating system VMM's to carry out the actual work in their
respective kernels. In this way, we do not replace the OS's VMM, we enhance it.

#### coherency

Developers need to know about memory coherency models to write *correct*
applications. The coherency model is how we expect memory to behave when we
read from and write to it.

Note, memory *coherency* and memory *consistency* often refer to the same
things in the field of distributed shared memory systems. This document
continues that trend. Feel free to submit a PR if I'm completely wrong.

There are lots of different types of memory coherency models:

  - [strict](http://en.wikipedia.org/wiki/Consistency_model#Strict_Consistency)
  - [sequential](http://en.wikipedia.org/wiki/Consistency_model#Sequential_consistency)
  - [processor](http://en.wikipedia.org/wiki/Consistency_model#Processor_Consistency)
  - [release](http://en.wikipedia.org/wiki/Consistency_model#Release_Consistency)
  - [lazy-release](http://en.wikipedia.org/wiki/Consistency_model#Release_Consistency)
  - *[and many, many more...](http://en.wikipedia.org/wiki/Consistency_model)*

Strong models often guarantee a more atomic model (i.e., a memory will read the
value that was last written to it). Such strong guarantee come at the cost of
*performance*.

Weak models often take a more lax model (i.e., a memory will read the value
that was last written to it *soon*). Such a weak guarantee better *performance*
at the cost of *correctness*.

_TODO(sholsapp):_ Explain how coherency becomes interesting as we scale from
one execution thread to multiple execution threads, and how it becomes even
more interesting as we split the address space accross multiple machines. This
will involve a discussion of the pthread implementation.

### consensus

All operations against any shared stateful object, i.e., the distributed shared
memory infrastructure's VMM, must be agreed upon by the implementation-defined
relevant owner set.

## implementation

### memory

There are fundamentally two different regions of memory that a distributed
shared memory system needs to consider:

  - local internal memory
  - shared application memory

#### local internal memory

The internal memory region can also be referred to as local internal memory.
This region of memory is a good place to put local threads, process-specific
data, and other things that are local to a specific process running the shared
application. Use of memory by the distributed system itself should strive to
use this memory region for any operations that require memory.

#### shared application memory

The application memory is the region of memory that is managed by the page
table and actually used by the shared application, i.e., this region of memory
is home to the shared application's *heap*.

This region of memory must also be synchronized between processes running the
shared application. This region of memory's *consistenty model* need not be the
same as the model used to maintain the *shared page table* from the previous
section.

### owners and companies

Every page of memory in the system has an owner, a company, and one or more
leases.

#### owners

Owners are the atomic entities that compose companies. Owners primarily
responsible for servicing requests from clients. When servicing requests that
require coordination, it is the owner's responsibility to get consensus in such
a way that exhibits [Byzantine fault
tolerance](https://en.wikipedia.org/wiki/Byzantine_fault_tolerance) and
[Paxos-like](https://en.wikipedia.org/wiki/Paxos_(computer_science)) consensus,
hereby referred to simply as *consensus*. This system makes use of the
[Raft](https://raft.github.io/) consensus algorithm to maintain the shared data
structure due to its simplified design and implementation compared to
traditional Paxos.

It's appropriate to say that an *owner* gets *consensus* of the *company* for
some operation. Consensus is usually mandatory when dealing with shared data
structures that have to do with the internal correctness of the application
(hint: the page table data structure discussed in the future is one such shared
data structure that must always be correct to maintain the correctness of the
application, and therefor all operations on it must have consensus of the
respective company).

#### companies

A company is a set of owners that act as managers of the application memory
page. A company typically manages a range of memory, not just a single memory
page. A company can be thought of as a shard in that a company will only manage
an agreed upon region of memory, thereby limiting the amount of overall work
that any single company is accountable for.

A company is responsible for maintaining a consistent version of metadata,
granting and revoking permissions, ensuring redundancy, servicing requests, and
any other responsibilities that might relate to a page of memory.

A company is run by a single owner at a time. The owner that is running a
company at some given time is referred to as the leader. The leader of a
company is the same leader implementation referred to in the
[Raft](https://raft.github.io/) consensus algorithm. A company can grow and
shrink depending on the number of owners available in the system.  Companies
are calculated using the size of the global heap and the number of potential
owners (i.e., nodes)  available in the system.

### distributed state machine

All members of the cluster are custodians of a distributed state machine,
hereby referred to as the *page table*.

The *page table* is a data structure that can be used to reason about the
memory that exists in the application. This data structure is maintained on
each node in an in-memory SQLite database.

It is important to note that the page table on any specific host *only tracks
the memory that is or was available on that host*. Thus, the page table only
exists in parts on each node.

#### operations

The *page table* is a distributed state machine, and as such, supports a set of
operations that can be applied to it to transition itself between states. Some
of these operations include:

- allocate memory
- lease memory

Each of these operations happen over a period of time forming a *log*. This
*log* must be kept consistent between companies. A hypothetical *log* snippet
might look something like the following:

```
...
S1 allocate 0x1000
S1 allocate 0x2000
S2 allocate 0x3000
```

Such a log ultimately corresponds to a data structure, the *page table*, that
allows us to get and set information about application memory in a way that is
consistent between all owners in a company..

##### allocate memory

When an application calls malloc, the application heap receives the call. From
the top down, each heap layer of the application heap will attempt to service
the call. A high level heap layer may successfully service the call if, for
example, it was maintaining a free list of memory. Once a layer successfully
services the call, the application's call to malloc returns. If the page table
heap layer attempts to service the call, the following algorithm is used.

The page table heap layer checks if there is sufficient physical memory left on
the current host. If there is not sufficient memory on the current host, the
page table heap layer unmaps memory that it does not own and then proceeds as
if there was sufficient memory on the host. If there is sufficient memory on
the current host, the page table heap layer maps the memory into the address
space and begins negotiations with each owner in the owner set. Each owner in
the owner set will respond with one of the following responses during
negotiations:

  HTTP 200 - The owner agrees to the terms and has successfully committed the
  metadata to its pagetable.

Execution blocks until negotiations complete with an appropriate number of
owners. The appropriate number of owners to require negotiations with is
determined by the coherency protocol the system is configured to use. Upon
completed negotiations, the new lease metadata is committed into the page table
and the application's call to malloc returns.


##### lease memory

When an application accesses a memory address that it does not have access to,
a SIGSEGV signal is raised in the thread that caused the fault to occur.
