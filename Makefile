CC = cc
CXX = c++
AR = ar rcs

CFLAGS = -std=c++11 -Wall -fPIC -DPIC -g -O2 -Wno-unused-local-typedefs
LDFLAGS = -pthread
DFLAGS = -shared

OS=$(shell uname | tr 'a-z' 'A-Z')
ARCH=$(shell uname -p)
DYEXT = so


ifeq ($(OS),LINUX)
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_LINUX
endif

ifeq ($(OS),DARWIN)
AR = libtool -static -o
DFLAGS += -dynamiclib
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_MAC
DYEXT = dylib
endif

ifeq ($(ARCH),arm)
ifeq ($(OS),DARWIN)
CFLAGS += -DWEBRTC_ARCH_ARM64 -DWEBRTC_HAS_NEON
else
CFLAGS += -DWEBRTC_ARCH_ARM_V7 -DWEBRTC_ARCH_ARM -DWEBRTC_HAS_NEON
endif
endif


include common.mak
