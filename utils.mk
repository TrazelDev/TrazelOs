include commons.mk


LIBS_NAMES := $(shell find $(DIR_UTILS) -mindepth 1 -maxdepth 1 -type d -printf "%f ")
BIN_ALL_LIBS := $(addprefix $(DIR_BIN_UTILS)/, $(LIBS_NAMES))
export BIN_ALL_LIBS := $(addsuffix .a, $(BIN_ALL_LIBS))

LIBC_TZ := libc_tz
export DIR_BIN_LIB_LIBCTZ := $(DIR_BIN_UTILS)/$(LIBC_TZ)
export BIN_LIB_LIBCTZ := $(DIR_BIN_UTILS)/$(LIBC_TZ).a
