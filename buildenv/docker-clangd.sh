#!/bin/bash

HOST_PATH="$(pwd)"
CONTAINER_PATH="/mnt/TrazelOs"

docker exec -i trazelos-dev clangd \
    --path-mappings="${HOST_PATH}=${CONTAINER_PATH}" \
    "$@"
