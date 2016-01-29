# cthulhu

A distributed system testing framework.

# TODO

- Setup symlinks between IP address directories and the instance numbers. We
can reason about instances numbers in docker cmdline, but not on bash shell,
which is annoying.

- Find and move coredumps out of the directory. Make the corefile pattern
easier to modify.

- Command to stop and start containers (this doesn't work now because of the
instance tag that we give to each container, it is "already in use").
