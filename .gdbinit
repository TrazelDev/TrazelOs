define qemu-reset
	shell timeout 0.2 bash -c 'echo "system_reset" | nc localhost 55555'
	continue
end

define r
	qemu-reset
end
define run
	qemu-reset
end

# Connecting to the os:
set architecture i386:x86-64
set disassembly-flavor intel
target remote localhost:1234

# Setting the break point to main:
break kmain
continue

# C code window:
layout src
