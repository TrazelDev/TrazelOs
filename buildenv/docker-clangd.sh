#!/bin/bash

HOST_PATH="$(pwd)"
CONTAINER_PATH="/mnt/TrazelOs"

docker exec -i trazelos-dev clangd-15 \
    --path-mappings="${HOST_PATH}=${CONTAINER_PATH}" \
    "$@"
