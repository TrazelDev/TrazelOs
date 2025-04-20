# Trazel operating system - specializes mainly in managing memory

## An operating system focused on memory management using paging, virtual memory, and a dynamic memory pool (heap).
> [!Note]
> The project is currently undergoing a bootloader refactor; the kernel is not loaded at this stage.

## Building:
> [!NOTE]
> The project only supports Linux build process.

### Dependencies:
* Make
* Qemu
* Docker

### Build commands:
* Run the `build` script to spawn a docker container with all the tools.
* Inside of the docker run `make build`.

## Running:
* `make run`
> [!NOTE]
> If the docker uses `sudo` privileges you must use `sudo make run`.

## Cleaning:
* `make clean`

## Goals:
- [x] mbr
- [x] bootloader
- [x] idt
- [x] physical memory manager
- [x] virtual memory manager
- [x] process free memory (heap)
- [ ] process management
- [ ] file system
