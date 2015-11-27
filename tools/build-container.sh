#!/bin/bash

if [ $# -eq 0 ]
then
  echo "Specify a name for the container to build, e.g., docker-example."
  exit 1
fi

docker build -t $1 -f tools/Dockerfile .
