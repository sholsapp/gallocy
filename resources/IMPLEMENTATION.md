# gallocy

## implementation

### Application allocates memory

When an application calls malloc, the application heap receives the call. From
the top down, each heap layer of the application heap will attempt to service
the call. Once a layer services the call, the application's call to malloc
returns. If the page table heap layer attempts to service the call, the
following algorithm is used.

The page table heap layer checks if there is sufficient physical memory left on
the current host. If there is not sufficient memory on the current host, the
page table heap layer unmaps memory that it does not own and then proceeds as
if there was sufficient memory on the host. If there is sufficient memory on
the current host, the page table heap layer maps the memory into the address
space and begins negotiations with each owner in the owner set. Each owner in
the owner set will respond with one of the following responses:

  HTTP 200 - The owner agrees to the terms and has successfully committed the
  metadata to its pagetable. The response is empty.

  HTTP 405 - The owner disagrees to the terms because the metadata already
  exists in its pagetable. The response contains the lease information about
  the owner of the requested memory. The requester should update its pagetable
  and retry servicing the allocation from the beginning.

Execution blocks until negotiations complete with an appropriate number of
owners. The appropriate number of owners to require negotations with is
determined by the coherency protocol the system is configured to use. Upon
completed negotations, the new lease metadata is committed into the page table
and the application's call to malloc returns.


### Application faults on memory

When an application accesses a memory address that it does not have access to,
a SIGSEGV signal is raised in the thread that caused the fault to occur.
