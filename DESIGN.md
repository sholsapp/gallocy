# gallocy

Building a distributed shared memory infrastructure is *easy*.

## design

At a high level, the thigns a distributed shared memory infrastructure
architect cares about are:

  - memory
  - coherency
  - communication
  - ...

### memory

There are fundamentally three different regions of memory that a dsm needs to
consider:

  - local internal memory
  - a shared page table
  - shared application memory

Additionally, some type of arbiter needs to coordinate a common address space
using a shared page table. In traditional operating systems, this would
probably be called the ``virtual memory manager``.

#### local internal memory

The internal memory region can also be referred to as local internal memory.
This region of memory is a good place to put local threads, process-specific
data, and other things that are local to a specific process running the shared
application.

This is the *only* region of memory that is allowed to differ between local
processes running the shared application, and it is:

  - not included in the shared page table
  - not shared with any other process running the shared application

#### shared page table

The page table must be maintained so that the properties of the shared
application memory can be maintained. This page table can tell us things like
what memory is currently mapped in the application, which process participating
in running the shared application owns this memory, or what the local
permissions on that memory block currently are (e.g., ``PROT_READ``,
``PROT_WRITE``, ``PROT_EXEC``).

This page table must be maintained in a dedicated memory region so that it may
easily synchronized between processes running the shared application. The most
interesting part related to maintaining this shared data structure is how it is
kept updated, referred to as the *consistency model*, because of its
implications on *performance* and *correctness*.

#### shared application memory

The application memory is the region of memory that is managed by the page
table and actually used by the shared application, i.e., this region of memory
is home to the shared application's *heap*.

This region of memory must also be synchronized between processes running the
shared application. This region of memory's *consistenty model* need not be the
same as the model used to maintain the the *shared page table* from the
previous section.

### coherency

Developers need to know about memory coherency models to write *correct*
applications. The coherency model is how we expect memory to behave when we
read and write it.

Memory *coherency* and memory *consistency* often refer to the same things in
the field of distributed shared memory systems. This document continues that
trend. Feel free to submit a PR if I'm completely wrong.

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

Weak models often take a more lax model (i.e., a memory will read teh value
that was last written to it *soon*). Such a weak guarantee better *performance*
at the cost of *correctness*.

It doesn't take a rocket scientist to imagine why keeping memory strictly
coherent in a distributed shared memory system running 100,000 nodes would be
expensive. This is why distributed shared memory systems tend to use weaker
coherency models.

### communication

Communication leads to coordination, and that's an important area in
distributed systems.

This section talks about the nuances of how a process running a shared
application does things like:

  - join the group
  - leave the group
  - create a thread
  - join a thread
  - ask for a memory
  - give up a memory
