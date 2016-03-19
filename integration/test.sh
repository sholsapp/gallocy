#!/usr/bin/env bats


setup() {
  echo "SETUP"
  ROOT="$(pwd)"
  BIN="$ROOT/install/bin"
}


teardown() {
  echo "TEARDOWN"
}


@test "http server binary exists" {
  run ls "${BIN}/server"
  [ $status -eq 0 ]
}


# The below integration tests depend on a cthulhu-fixture being present. The
# fixture is created for 5 nodes.


@test "cthulu-fixture exists" {
  run ls "$ROOT/cthulhu-fixture"
  [ $status -eq 0 ]
}


@test "cthulhu-fixture starts/stops" {
  run "$ROOT/cthulhu-fixture/control" start
  [ $status -eq 0 ]
  run "$ROOT/cthulhu-fixture/control" stop
  [ $status -eq 0 ]
}


@test "leader election" {
  run "$ROOT/cthulhu-fixture/control" start
  [ $status -eq 0 ]
  # Wait for the cluster to elect a leader.
  sleep 5
  run python "$ROOT/integration/helpers/leader_election.py"
  echo $output
  [ $status -eq 0 ]
  run "$ROOT/cthulhu-fixture/control" stop
  [ $status -eq 0 ]
}
