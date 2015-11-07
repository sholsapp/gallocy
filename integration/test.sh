#!/usr/bin/env bats


@test "http server starts" {
  BIN=install/bin
  ${BIN}/httpd &
  PID=$!
  result=$(curl http://localhost:8080/admin)
  kill -9 $PID
  [ "$result" = "{\"foo\": \"bar\"}" ]
}
