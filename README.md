# Trazel operating system - specializes mainly in managing memory
## OS that mainly focuses on managing memory using paging, virtual memory, and a free memory pool for dynamic allocation of memory.

## Building:
> [!NOTE]
> The project will work on Linux out of the box (mainly Ubuntu, the OS it was developed with); it was not tested on Windows and Mac.

### Installation:

#### Ubuntu:
1. clone the projecct
2. cd project
3. sudo apt install `make`
4. make `dependencies`
5. cd `_linker_compiler_installer`
6. make `compiler_and_linker_dependencies`
7. in the case that it fails to run add the following to the bashrc PATH: "/usr/local/x86_64elfgcc/bin" with the command:
`echo` 'export PATH="$PATH:/usr/local/x86_64elfgcc/bin"' >> ~/.bashrc;

#### Other Linux distributions:
1. clone the projecct
2. cd project
3. install `make` `qemu-system-x86` `nasm`
4. cd `_linker_compiler_installer`
5. make `compiler_and_linker_dependencies`
6. in the case that it fails to run add the following to the bashrc PATH: "/usr/local/x86_64elfgcc/bin" with the command:
`echo` 'export PATH="$PATH:/usr/local/x86_64elfgcc/bin"' >> ~/.bashrc;

### Running the project:
#### Regular configuration:
cd project
make run
#### Debug configuration:
cd project
make run

### Cleaning the bin files:
cd project
make clean


## Current Goals:
- [x] mbr
- [ ] bootloader
- [ ] idt
- [ ] physical memory manager
- [ ] virtual memory manager
- [ ] process free memory (heap)
- [ ] process managemnt
- [ ] filing system