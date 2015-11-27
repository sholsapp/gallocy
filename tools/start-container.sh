#!/bin/bash

if [ $# -eq 0 ]
then
  echo "Must specify an instance number (0, 1, 2, ...)!"
  exit 1
fi

INSTANCE=$(docker run     \
  --interactive           \
  --tty                   \
  --detach                \
  --publish=808${1}:8081  \
  --hostname=gallocy${1}  \
  --memory=64M            \
  gallocy-example || exit 1)

IPADDR="10.0.0.${1}"
PIPEWORK_BIN="$(pwd)/tools/pipework"
PIPEWORK_OUTPUT=$("${PIPEWORK_BIN}" br1 \
  -i eth1                \
  -l app${1}             \
  ${INSTANCE}            \
  ${IPADDR}/24 || exit 1)

echo $PIPEWORK_OUTPUT
echo "Started ${INSTANCE} listening on ${IPADDR} exposed on 808${1}."
