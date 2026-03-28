# Trazel operating system - specializes mainly in managing memory

## An x86_64 bootloader and operating system build in c and compiled fully within a docker with cross compiler.
> [!Note]
> The project is currently undergoing refactor.

## Building:
> [!NOTE]
> The project only supports Linux build process but builds and runs in a docker,
> so setting up in windows will be as strait forward as creating a corsponding small bat script to launch docker.
### Dependencies:
* Docker
* xhost

### Build commands:
* Run the `build.sh` script to spawn a docker container with all the tools.
* **This script executes `make build` by default.**
* Inside of the docker run `make build` to build again.
* Alternatively, you can use the rebuild option `make rebuild` from inside the container.

## Running:
* Run the `build.sh` script to spawn a docker container with all the tools.
* Inside of the docker run `make run`.

## Cleaning:
* Run the `build.sh` script to spawn a docker container with all the tools.
* `make clean`

## Debugging:
> [!Note]
> When debugging for first time, the following: `add-auto-load-safe-path path-to-TrazelOs`,
> has to be added to the global .gdbinit file.
* Run the `build.sh` script to spawn a docker container with all the tools.
* `make debug` to run a debugging session inside of the docker.
* Inside regular terminal use `make connect` or `make debug_connect` to attach to the gdb session.

## Projects custom bootloader vs limine bootloader:
> [!NOTE]
> The project is booting with costume bootloader which will incremtly try to support limine boot protocol.
> For now the default bootloader is the custom bootloader.
#### To choose the limine bootloader specifically there are couple of option:
1. Use the limine corresponding options called: `runl`, `buildl` and `debugl`.
2. Set the BOOT_OPTION parameter upon running make, for example: `make run BOOT_OPTION=limine`.
3. Change the variable BOOT_OPTION to limine in the config.mk file.

## LSP support:
> Once the `build.sh` docker container has spawned, you can use `buildenv/docker-clangd.sh` as a clangd instance.
> It runs inside the docker and properly handles the cross compiler and other unique setup configurations.
