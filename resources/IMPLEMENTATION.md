# gallocy

## implementation

### owners, companies, and leases

Every page of memory in the system has an owner, a company, and one or more
leases.

#### owners

Owners are the atomic entities that compose companies. Owners primarily
responsible for servicing requests from clients. When servicing requests that
require coordination, it is the owner's responsibility to get consensus in such
a way that exhibits [Byzantine fault
tolerance](https://en.wikipedia.org/wiki/Byzantine_fault_tolerance) and
[Paxos-like](https://en.wikipedia.org/wiki/Paxos_(computer_science) consensus,
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
page. This company is responsible for maintaining a consistent version of
metadata, granting and revoking permissions, ensuring redundancy, servicing
requests, and any other responsibilities that might relate to a page of memory.
A company can grow and shrink depending on the number of owners available in
the system. Companies are calculated using the size of the global heap and the
number of potential owners (i.e., nodes)  available in the system.

#### leases

A lease is a temporary grant from a company to read or write a part of
application memory that the company owns. Leases are granted from individual
owners. A lease has a set of terms that must be followed until the lease
expires or is forfeited.

#### interactions as a distributed log

Owners, companies, and leases form a distributed state machine that can be
maintained in as  distributed log.


### Page table

The page table is a data structure that can be used to reason about the memory
that exists in an application, the memory permissions, and more. This data
structure is maintained on each node in an in-memory SQLite database that
utilizes memory allocated by the shared page table allocator.

The page table is a SQLite database table that has columns for the following
pieces of information::

  - The address of the page of memory.
  - The total number of faults.
  - The owner set of the page.
  - The lease holder of the page.
  - The current memory protections.

It is important to note that the page table on any specific host *only tracks
the memory that is or was available on that host*. Thus, the page table only
exists in parts on each node. To assemble the entire address space of the
memory region maintained by the system, one must employ a scatter/gather
operation to query at least one owner of each owner set that exists.

### Application allocates memory

When an application calls malloc, the application heap receives the call. From
the top down, each heap layer of the application heap will attempt to service
the call. A high level heap layer may successfully service the call if, for
example, it was maintaining a free list of memory.. Once a layer successfully
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

  HTTP 301 - The owner is no longer an authoritative owner of this region of
  memory but knows another host that is. The requester should follow the
  redirect and update its routing information. The payload contains additional
  routing information that the requester can use to patch its own routing
  information.

  HTTP 302 - The owner has leased this region of memory to another host. The
  requester should follow the redirect but make future requests to this owner.

  HTTP 409 - The owner disagrees to the terms because the metadata already
  exists in its pagetable. The response contains the lease information about
  the owner of the requested memory. The requester should update its pagetable
  and retry servicing the allocation from the beginning.

  HTTP 503 - The owner is busy negotiating with other requesters so this
  requester should try again at a future time. The response data may indicate
  that the requester need not retry the request at a future time.

Execution blocks until negotiations complete with an appropriate number of
owners. The appropriate number of owners to require negotiations with is
determined by the coherency protocol the system is configured to use. Upon
completed negotiations, the new lease metadata is committed into the page table
and the application's call to malloc returns.


### Application faults on memory

When an application accesses a memory address that it does not have access to,
a SIGSEGV signal is raised in the thread that caused the fault to occur.
