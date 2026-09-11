include drivers.mk
include commons.mk
include utils.mk
include config.mk

.PHONY: clean run build debug_connect connect

# Running flags:
ifeq ($(RUN_ONLY), 0)
RUN_DEPENDENCIES := build
endif
QEMU := qemu-system-x86_64
QEMU_FLAGS :=
QEMU_DEBUG_FLAGS := -monitor tcp:0.0.0.0:55555,server,nowait -s -S
ifeq ($(HEADLESS),0)
	QEMU_FLAGS += -serial stdio
else
	QEMU_FLAGS += -nographic
endif
ifeq ($(BIOS_OR_UEFI),uefi)
	QEMU_FLAGS += -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd
	QEMU_FLAGS += -drive if=pflash,format=raw,file=/usr/share/OVMF/OVMF_VARS.fd
endif
QEMU_FLAGS += -drive format=raw,file=$(OS_IMG)

run: $(RUN_DEPENDENCIES)
	$(QEMU) $(QEMU_FLAGS)
debug: $(RUN_DEPENDENCIES)
	$(QEMU) $(QEMU_FLAGS) $(QEMU_DEBUG_FLAGS)

build:
	$(MAKE) -f os_img.mk $(OS_IMG)
connect: debug_connect
debug_connect:
	gdb bin/kernel.bin
rebuild:
	$(MAKE) clean
	$(MAKE) build
clean:
	rm -rf bin

# auto choosing limine options:
runl:
	$(MAKE) run BOOT_OPTION=limine
buildl: 
	$(MAKE) build BOOT_OPTION=limine
debugl:
	$(MAKE) debug BOOT_OPTION=limine
rebuildl:
	$(MAKE) rebuild BOOT_OPTION=limine
