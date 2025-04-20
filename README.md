# Trazel operating system - specializes mainly in managing memory

## OS that mainly focuses on managing memory using paging, virtual memory, and a free memory pool for dynamic allocation of memory.
> [!Note]
> The project is currently under bootloader refactor therefor the kernel is not loaded currently

## Building:
> [!NOTE]
> The project only supports linux build

### Dependecies:
* Make
* Qemu
* Docker

### Build commands:
* Run the `build` script to spawn a docker container with all the tools
* Inside of the docker run `make build`

## Running:
* `make run`
> [!NOTE]
> If the docker uses `sudo` privileges you must use `sudo make run`

## Cleaning:
* `make clean`

## Goals:
- [x] mbr
- [x] bootloader
- [x] idt
- [x] physical memory manager
- [x] virtual memory manager
- [x] process free memory (heap)
- [ ] process managemnt
- [ ] filing system
