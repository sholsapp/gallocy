# about

This module contains wrappers and reimplementations of code in glibc that
internally use malloc. These wrappers and reimplementations ensure that this
library's use of glibc does not mistakenly pollute the target application's
address space.

See [Sanos source code](http://www.jbox.dk/sanos/source/) for simple
implementations of many of glibc's interface.
