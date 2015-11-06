#!/bin/bash

if [ $# -eq 0 ]
then
  echo "Must specify an instance number (0, 1, 2, ...)!"
  exit 1
fi

docker network create -d bridge my-net || exit 1

docker run                \
  --detach                \
  --publish=808${1}:8080  \
  --hostname=gallocy${1}  \
  --memory=64M            \
  --net=my-net            \
  gallocy-example
