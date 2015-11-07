#!/usr/bin/env bats


setup() {
  echo "FOO"
}


teardown() {
  echo "BAR"
}


@test "http server binary exists" {
  BIN=install/bin
  run ls "${BIN}/httpd"
  [ $status -eq 0 ]
}


@test "http server starts" {
  BIN=install/bin
  ${BIN}/httpd &
  PID=$!
  echo "Started server on ${PID}"
  run curl --max-time 1 http://localhost:8080/admin
  echo "Killing ${PID}"
  kill -9 $PID
  [ "$result" = "{\"foo\": \"bar\"}" ]
}
