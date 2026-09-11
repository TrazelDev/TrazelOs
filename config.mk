# limine - for limine bootloader, custom - my custom bootloader:
BOOT_OPTION ?= limine

# bios/uefi - only relevant if this is limine bootloader
BIOS_OR_UEFI ?= bios

# 1 - headless build, 0 - gui build:
HEADLESS ?= 0

# 1 - only run the os in qemu, 0 - build and run:
RUN_ONLY ?= 0

ifneq ($(BOOT_OPTION),limine)
ifneq ($(BOOT_OPTION),custom)
$(error BOOT_OPTION in config.mk must be either 'limine' or 'custom', got '$(BOOT_OPTION)')
endif
endif

ifneq ($(BIOS_OR_UEFI),uefi)
ifneq ($(BIOS_OR_UEFI),bios)
$(error BIOS_OR_UEFI in config.mk must be either 'bios' or 'uefi', got '$(BIOS_OR_UEFI)')
endif
endif

ifeq ($(BIOS_OR_UEFI),uefi)
ifeq ($(BOOT_OPTION),custom)
$(error BIOS_OR_UEFI cannot be 'uefi' when BOOT_OPTION is 'custom', please set BOOT_OPTION=limine or set BIOS_OR_UEFI=bios in config.mk)
endif
endif

ifneq ($(HEADLESS),0)
ifneq ($(HEADLESS),1)
$(error HEADLESS in config.mk must be either 0 or 1, got '$(HEADLESS)')
endif
endif

ifneq ($(RUN_ONLY),0)
ifneq ($(RUN_ONLY),1)
$(error RUN_ONLY in config.mk must be either 0 or 1, got '$(RUN_ONLY)')
endif
endif
