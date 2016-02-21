# gallocy

## unit testing

To run unit tests, use the `project` helper.

```
./project build
```

Running `build` will implicitly call `test` and is suggested instead of using
`test` directly so that you ensure you're testing the latest version of your
code.

## distributed testing

To run the distributed tests, use the `project` helper to create a docker
container named `gallocy-example` with your latest changes. You'll also need to
check out and set up [cthulhu](https://github.com/corelabsio/cthulhu).

```
./project docker
cthulhu --instances 5 --docker-container gallocy-example
cd cthulhu-fixture
```

Start the instances using the control script in the fixture.

```
./control start
```

This will start five instances of gallocy. You can check on the status of the
five instances using the
[docker](https://docs.docker.com/engine/reference/commandline/cli/) command
line. A few useful commands are `ps`, `log`, `start`, and `stop`.

```
docker ps
docker logs -f i001
docker stop i001
docker logs -f i002
docker start i001
```

When you're finished, stop the instances using the control script in the
fixture.

```
./control stop
```

### dealing with failures

Debugging a failure in one of the instances can be done using `gdb`. If a
failure occurs in one of the instances, a core file named `core.server` will be
generated in the instance's respective `var` directory.

The steps that you need to follow to inspect the core file are:

1. Set permissions appropriately on the core file *(it will be owned by root)*.
2. Move shared libraries to `/home/gallocy/lib` to match the file system of the
container *(you may need to create this directory if it does not already
exist)*.
3. Involve `gdb install/bin/server cthulhu-fixture/i001/bindmounts/var/core.server`
