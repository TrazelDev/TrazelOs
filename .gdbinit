# TODO: create a function to recompile all the code reload data in gdb and relaunch qemu instance
define qemu-reset
	tbreak kmain
	shell timeout 0.2 bash -c 'echo "system_reset" | nc localhost 55555'
	continue
end

define r
	qemu-reset
end
define run
	qemu-reset
end

define hook-quit
    save breakpoints bin/.gdb_bps
end

# loading init process file debug info:
add-symbol-file bin/user/init

# Connecting to the os:
set architecture i386:x86-64
set disassembly-flavor intel
target remote localhost:1234

# reloading previous breakpoints:
shell touch bin/.gdb_bps # making sure there is an empty file to make sure it will not crush
source bin/.gdb_bps

# Setting the break point to main:
tbreak kmain
continue

# C code window:
layout src
