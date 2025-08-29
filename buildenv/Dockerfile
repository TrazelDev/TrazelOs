# Getting the base ubntu image:
FROM ubuntu:22.04

# Updating the system and installing essentials:
RUN apt-get update && apt-get install -y build-essential

# Installing regular important tools
RUN apt-get -y install nasm

# Installing essential tools for a cross compiler:
RUN apt-get install -y bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev curl

ENV PREFIX=/mnt/cross_compiler/compiler
ENV PATH="$PREFIX/bin:$PATH"
ENV TARGET=x86_64-elf
RUN mkdir -p /mnt/cross_compiler
WORKDIR /mnt/cross_compiler
RUN mkdir -p compiler

# compiling the binutils:
RUN curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.35.1.tar.gz
RUN tar xf binutils-2.35.1.tar.gz
RUN mkdir -p binutils-build
WORKDIR binutils-build
RUN ../binutils-2.35.1/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
RUN make all install 2>&1 | tee make.log
WORKDIR ..

# compiling the compiler:
RUN curl -O https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz
RUN tar xf gcc-10.2.0.tar.gz
RUN mkdir -p gcc-build
WORKDIR gcc-build
RUN ../gcc-10.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c++ --without-headers
RUN make all-gcc
RUN make all-target-libgcc
RUN make install-gcc
RUN make install-target-libgcc


# Making sure there is access to the compiler everywhere:
ENV PATH="$PATH:/mnt/cross_compiler/compiler/bin"


# Extra dependencies:
RUN apt-get install -y fdisk
RUN apt-get install -y qemu qemu-system
RUN apt-get install -y mesa-utils mesa-utils-extra libgl1 libgl1-mesa-dri
RUN apt-get install -y mtools dosfstools

# mounting the actuall project:
RUN mkdir /mnt/TrazelOs
WORKDIR /mnt/TrazelOs
