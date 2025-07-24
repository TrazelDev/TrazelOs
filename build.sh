#!/bin/bash

# building:
docker build buildenv -t os-build-env

# running:
xhost +local:root
docker run -it -e DISPLAY=$DISPLAY -e XDG_RUNTIME_DIR=/tmp/runtime-root \
	-v /tmp/.X11-unix:/tmp/.X11-unix \
	-v $(pwd):/mnt/TrazelOs \
	os-build-env:latest sh -c "umask 0000 && exec sh"
