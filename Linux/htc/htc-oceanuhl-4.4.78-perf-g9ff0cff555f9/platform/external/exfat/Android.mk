LOCAL_PATH:= $(call my-dir)

libexfat_src_files := \
	libexfat/cluster.c \
	libexfat/io.c \
	libexfat/log.c \
	libexfat/lookup.c \
	libexfat/mount.c \
	libexfat/node.c \
	libexfat/time.c \
	libexfat/utf.c \
	libexfat/utils.c

libexfat_c_includes := \
	external/exfat/libexfat \

include $(CLEAR_VARS)
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_MODULE:= mkexfat
LOCAL_CLANG := true
LOCAL_SRC_FILES := \
	mkfs/cbm.c \
	mkfs/fat.c \
	mkfs/main.c \
	mkfs/mkexfat.c \
	mkfs/rootdir.c \
	mkfs/uct.c \
	mkfs/uctc.c \
	mkfs/vbr.c \
	$(libexfat_src_files)
LOCAL_C_INCLUDES := $(libexfat_c_includes)

#For 64-bit arm
#LOCAL_CONLYFLAGS := -Werror -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter
#For 32-bit arm
LOCAL_CFLAGS := -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-parameter -D_FILE_OFFSET_BITS=64
LOCAL_CONLYFLAGS := -std=c11
include $(BUILD_EXECUTABLE)
