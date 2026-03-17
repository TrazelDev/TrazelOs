# Setting up the kernel source files directory: 
directory kernel

# Connecting to the os:
set architecture i386:x86-64
target remote localhost:1234

# Setting the break point to main:
break kmain
continue

# C code window:
layout src
