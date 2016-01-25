# api

## implicit

An implicit API is an API that requires no change to the target application,
i.e., the application to be run on the distributed shared memory system. This
approach depends on the underlying operating system providing mechanisms to
override memory and threading behavior.

The Linux operating system provides mechanisms for overriding memory and
threading behavior using function interposition. The following is an example of
function interposition.

```
extern "C" int pthread_create(pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg) {
  pthread_create_function __original_pthread_create =
    reinterpret_cast<pthread_create_function>
    (reinterpret_cast<uint64_t *>(dlsym(RTLD_NEXT, "pthread_create")));
  // ...
  return __original_pthread_create(thread, attr, start_routine, arg);
}
```

This sample code demonstrates how one can provide a custom implementation of
the `pthread_create` function. The demonstration shows that we can provide a
replacement for `pthread_create` simply by redefining symbol. At the same time,
this demonstration shows us that we can obtain a handle to the original
redefined function using the `dlsym` function  in case the original
implementation is needed. In this way, one can provides alternative
implementations for all memory and threading behavior, thereby creating an
implicit programming interface to the target application.

Providing an implicit API has several problems to the gallocy designers that
are challenging to resolve. The most challenging problem is the fact that by
interposing a function, one interposes that function in both the target
application *and* the gallocy framework itself. This fact makes it challenging
to share, for example, libraries like `libc` and `libc++`, which both make
ample use of functions such as `malloc`, in such a way that allows one to
distinguish between the caller (recall that the gallocy framework must maintain
a clean separation of its internal memory use from the target application's
memory use).

One solution to this problem would require providing and using a custom `libc`
and `libc++` implementation that makes use of the gallocy framework's internal
memory interface, and leaving the original `libc` and `libc++` to the target
application. This is burdensome to maintain but would allow the gallocy
designers to maintain a clean separation of internal and application memory.

Further investigation may reveal that statically compiling a `libc` and
`libc++` implementation with hidden symbols would offer an equivalent and
maintainable solution.

## explicit

An explicit API is an API that requires the target application explicitly use
the provided API. This approach depends solely on the target application to use
and adhere to the provided API and has no dependencies on the underlying
operating system.

A sample API would include replacement memory and threading functions: instead
of using `malloc` one would use `gallocy_malloc`, instead of using
`pthread_create` one would use `gallocy_pthread_create`, and so on and so
forth.

Providing an explicit API dramatically decreases the challenges associated with
implicit API, but pushes more challenges towards the application developer. The
explicit API requires that applications be written with the gallocy API. For
anything but the most trivial applications, this is a problem: any use of
library code, including `libc` and `libc++`, would not work without alteration.
