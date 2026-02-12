# Trazel operating system - specializes mainly in managing memory

## An operating system focused on memory management using paging, virtual memory, and a dynamic memory pool (heap).
> [!Note]
> The project is currently undergoing a bootloader refactor;

## Building:
> [!NOTE]
> The project only supports Linux build process but runs in a docker,
> so making it work on windows won't be a problem

### Dependencies:
* Docker
* xhost

### Build commands:
* Run the `build.sh` script to spawn a docker container with all the tools.
* Inside of the docker run `make build`.

## Running:
* Run the `build.sh` script to spawn a docker container with all the tools.
* Inside of the docker run `make run`.

## Cleaning:
* Run the `build.sh` script to spawn a docker container with all the tools.
* `make clean`
