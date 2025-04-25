include commons.mk

export LIBS_NAMES != ls $(DIR_UTILS) -d
export BIN_ALL_LIBS := $(addprefix $(DIR_BIN_UTILS), $(addsuffix .a, $(LIBS_NAMES)))

LIBC_TZ := libc_tz
export DIR_BIN_LIB_LIBCTZ := $(DIR_BIN_UTILS)/$(LIBC_TZ)
export BIN_LIB_LIBCTZ := $(DIR_BIN_UTILS)/$(LIBC_TZ).a
