#!/bin/bash

docker build buildenv -t os-build-env
docker run -it -v $(pwd):/mnt/TrazelOs os-build-env:latest
