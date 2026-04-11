#!/bin/bash

# building:
docker build buildenv -t os-build-env

# running:
xhost +local:root
docker run -it --rm --name trazelos-dev \
	-e DISPLAY=$DISPLAY                   \
	-p 1234:1234                          \
	-p 55555:55555                        \
	-e XDG_RUNTIME_DIR=/tmp/runtime-root  \
	-v /tmp/.X11-unix:/tmp/.X11-unix      \
	-v $(pwd):/mnt/TrazelOs               \
	os-build-env:latest sh -c "umask 0000 && cd /mnt/TrazelOs && make clean && bear -- make build ; exec bash"
