
compiler_and_linker_dependencies:
	sudo apt update
	sudo apt install python3
	sudo apt install build-essential
	sudo apt install bison
	sudo apt install flex
	sudo apt install libgmp3-dev
	sudo apt install libmpc-dev
	sudo apt install libmpfr-dev
	sudo apt install texinfo
	sudo apt-get install curl

	cd ~
	export command is used to set up an enviroment variable
	# the PREFIX variable is used to specify the directory where the compiled program or library should be installed.
	# and it is specific to the programm. 
	export PREFIX="/usr/local/x86_64elfgcc"
	# In the context of the export command in Linux, TARGET is typically used as an environment variable. 
	# It is often used in scenarios where building code on one machine (the host) to runing it on a different machine (the target).
	export TARGET=x86_64-elf
	export PATH="$$PREFIX/bin:$$PATH"
	
	mkdir -p /tmp/src # the p means that if the dir olready exists not to throw an error
	cd /tmp/src
	curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.35.1.tar.gz
	tar xf binutils-2.35.1.tar.gz

	mkdir -p binutils-build
	cd binutils-build
	../binutils-2.35.1/configure --target=$$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$$PREFIX 2>&1 | tee configure.log 
	sudo make all install 2>&1 | tee make.log

	cd /tmp/src
	curl -O https://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz
	tar xf gcc-10.2.0.tar.gz
	mkdir -p gcc-build
	cd gcc-build
	../gcc-10.2.0/configure --target=$$TARGET --prefix="$$PREFIX" --disable-nls --disable-libssp --enable-languages=c++ --without-headers
	sudo make all-gcc
	sudo make all-target-libgcc
	sudo make install-gcc
	sudo make install-target-libgcc

	# making it so all of the things that have been installed will accessible from everywhere
    @if ! grep -Fxq "export PATH=\"$$PATH:/usr/local/x86_64elfgcc/bin\"" ~/.bashrc; then \
        echo 'export PATH="$$PATH:/usr/local/x86_64elfgcc/bin"' >> ~/.bashrc; \
    fi

	# pls restart your terminal. 
	# run this command after finishing the make setup: 
	# echo 'export PATH="$PATH:/usr/local/x86_64elfgcc/bin"' >> ~/.bashrc; \