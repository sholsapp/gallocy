# gallocy

## implementation

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
