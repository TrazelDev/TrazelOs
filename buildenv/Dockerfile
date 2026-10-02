# Getting the base ubntu image:
FROM ubuntu:22.04

# Updating the system and installing essentials:
RUN apt-get update && apt-get install -y build-essential

# Installing regular important tools
RUN apt-get -y install nasm

# Installing essential tools for a cross compiler:
RUN apt-get install -y bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev curl

ENV CROSS_COMPILER_PATH=/mnt/cross_compiler
ENV CROSSC_COMPILER_AUXILIARIES_PATH=/mnt/bin_auxilarys
ENV AUXILIARIES_TARBALLS_PATH=/mnt/bin_auxilarys/tarballs
RUN mkdir -p $CROSS_COMPILER_PATH
RUN mkdir -p $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p $AUXILIARIES_TARBALLS_PATH

# Installing autoconf:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
RUN tar xf autoconf-2.69.tar.gz
RUN mv autoconf-2.69 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/autoconf-2.69
RUN ./configure && make && make install

# Installing automake:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://ftp.gnu.org/gnu/automake/automake-1.15.1.tar.gz
RUN tar xf automake-1.15.1.tar.gz
RUN mv automake-1.15.1 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/automake-1.15.1
RUN ./configure && make && make install

ENV PREFIX=$CROSS_COMPILER_PATH
ENV PATH="$PREFIX/bin:$PATH"
ENV TARGET=x86_64-trazelos
RUN mkdir -p /tmp/patches
COPY binutils_patches/ /tmp/binutils_patches/

# compiling the binutils:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.35.1.tar.gz
RUN tar xf binutils-2.35.1.tar.gz
RUN mv binutils-2.35.1 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
# patching for my specific os
RUN patch -p0 --input /tmp/binutils_patches/config_sub.patch
RUN patch -p0 --input /tmp/binutils_patches/config_bfd.patch
RUN patch -p0 --input /tmp/binutils_patches/gas_configure_tgs.patch
RUN patch -p0 --input /tmp/binutils_patches/ld_configure_tgt.patch
RUN patch -p0 --input /tmp/binutils_patches/ld_Makefile_am_bak.patch
RUN cp /tmp/binutils_patches/elf_x86_64_trazelos.sh ./binutils-2.35.1/ld/emulparams/elf_x86_64_trazelos.sh
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/binutils-2.35.1/ld
RUN automake
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p bintuils_build
WORKDIR bintuils_build
RUN ../binutils-2.35.1/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX 2>&1 | tee configure.log
RUN make all install 2>&1 | tee make.log

# compiling the compiler:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz
RUN tar xf gcc-10.2.0.tar.gz
RUN mv gcc-10.2.0 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
# patching everything:
COPY gcc_patches/ /tmp/gcc_patches/
RUN patch -p0 --input /tmp/gcc_patches/config_sub.patch
RUN patch -p0 --input /tmp/gcc_patches/config_gcc.patch
RUN patch -p0 --input /tmp/gcc_patches/crossconfig_m4.patch
RUN patch -p0 --input /tmp/gcc_patches/libcgcc_config_host.patch
RUN patch -p0 --input /tmp/gcc_patches/mkfixinc_sh.patch
RUN cp /tmp/gcc_patches/trazelos.h ./gcc-10.2.0/gcc/config/trazelos.h
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/gcc-10.2.0/libstdc++-v3/
RUN autoconf
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p gcc-build
WORKDIR gcc-build
RUN ../gcc-10.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c++ --without-headers
RUN make all-gcc
RUN make all-target-libgcc
RUN make install-gcc
RUN make install-target-libgcc

# Installing autoconf for newlib:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://ftp.gnu.org/gnu/autoconf/autoconf-2.65.tar.gz
RUN tar xf autoconf-2.65.tar.gz
RUN mv autoconf-2.65 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p autoconf-2.65-bins
WORKDIR autoconf-2.65
RUN ./configure --prefix=$CROSSC_COMPILER_AUXILIARIES_PATH/autoconf-2.65-bins \
    --program-suffix=-2.65 && make && make install
ENV PATH="$CROSSC_COMPILER_AUXILIARIES_PATH/autoconf-2.65-bins/bin:$PATH"

# Installing automake for newlib:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://ftp.gnu.org/gnu/automake/automake-1.11.tar.gz
RUN tar xf automake-1.11.tar.gz
RUN mv automake-1.11 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p automake-1.11-bins
WORKDIR automake-1.11
RUN ./configure --prefix=$CROSSC_COMPILER_AUXILIARIES_PATH/automake-1.11-bins \
    --program-suffix=-1.11 && make && make install
ENV PATH="$CROSSC_COMPILER_AUXILIARIES_PATH/automake-1.11-bins/bin:$PATH"

# building newlib:
WORKDIR $AUXILIARIES_TARBALLS_PATH
RUN curl -O https://sourceware.org/pub/newlib/newlib-2.5.0.tar.gz
RUN tar xf newlib-2.5.0.tar.gz
RUN mv newlib-2.5.0 $CROSSC_COMPILER_AUXILIARIES_PATH
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
# patching everything:
COPY newlib_patches/ /tmp/newlib_patches/
RUN patch -p0 --input /tmp/newlib_patches/config_sub.patch
RUN patch -p0 --input /tmp/newlib_patches/configure_host.patch
RUN patch -p0 --input /tmp/newlib_patches/libc_sys_configure_in.patch
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/newlib-2.5.0/newlib/libc/sys
RUN autoconf-2.65
COPY newlib_libc/ /tmp/newlib_libc
RUN mkdir -p $CROSSC_COMPILER_AUXILIARIES_PATH/newlib-2.5.0/newlib/libc/sys/trazelos/
RUN cp /tmp/newlib_libc/* $CROSSC_COMPILER_AUXILIARIES_PATH/newlib-2.5.0/newlib/libc/sys/trazelos/
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH/newlib-2.5.0/newlib/libc/sys/trazelos
RUN AUTOMAKE=automake-1.11 ACLOCAL=aclocal-1.11 autoreconf-2.65
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir build-newlib
WORKDIR build-newlib
ENV SYSROOT=/mnt/sysroot
RUN ../newlib-2.5.0/configure --target=$TARGET --prefix=/usr --enable-newlib-io-long-long
RUN make all
RUN make DESTDIR=${SYSROOT} install
RUN cp -ar $SYSROOT/usr/$TARGET/* $SYSROOT/usr/
RUN mv $SYSROOT/usr/lib/crt0.o $SYSROOT/usr/lib/crt1.o

# Rebuilding binutils with libc in mind:
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p binutils_build_hosted
WORKDIR binutils_build_hosted
RUN ../binutils-2.35.1/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --enable-interwork --enable-multilib --disable-nls --disable-werror
RUN make all install

# Rebuilding gcc with libc in mind:
WORKDIR $CROSSC_COMPILER_AUXILIARIES_PATH
RUN mkdir -p gcc_build_hosted
WORKDIR gcc_build_hosted
RUN ../gcc-10.2.0/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT --disable-nls --disable-libssp --enable-languages=c,c++
RUN make all-gcc all-target-libgcc
RUN make install-gcc install-target-libgcc

# Making sure there is access to the compiler everywhere:
ENV PATH="$PATH:/mnt/cross_compiler/compiler/bin"


# Extra dependencies:
RUN apt-get update
RUN apt-get install -y fdisk
RUN apt-get install -y qemu qemu-system
RUN apt-get install -y mesa-utils mesa-utils-extra libgl1 libgl1-mesa-dri
RUN apt-get install -y mtools dosfstools
RUN apt-get install -y bear
RUN apt-get update
RUN apt-get install -y clangd-15
RUN apt-get install -y clang-tidy
RUN apt-get install -y parted


# Getting limine-bootloader:
WORKDIR /mnt 
RUN curl -O https://codeberg.org/Limine/Limine/releases/download/v10.8.5/limine-10.8.5.tar.gz
RUN tar -xzf limine-10.8.5.tar.gz
WORKDIR /mnt/limine-10.8.5
RUN ./configure CC_FOR_TARGET=x86_64-trazelos-gcc LD_FOR_TARGET=x86_64-trazelos-ld OBJCOPY_FOR_TARGET=x86_64-trazelos-objcopy OBJDUMP_FOR_TARGET=x86_64-trazelos-objdump READELF_FOR_TARGET=x86_64-trazelos-readelf --enable-bios --enable-uefi-x86-64
RUN make 
RUN make install

# mounting the actuall project:
RUN mkdir /mnt/TrazelOs
WORKDIR /mnt/TrazelOs
