CC = cc
CXX = c++
AR = ar rcs

CFLAGS = -std=c++11 -Wall -fPIC -DPIC -g -O2 -Wno-unused-local-typedefs
LDFLAGS = -pthread -shared
CPPFLAGS = -MMD -MP

#win32,macosx,linux, android,ios
OS =

#arm,arm64,-
ARCH =

DYEXT = so

ifeq ($(OS),)
$(error "usage: make OS=win32/macosx/linux or make OS=android/ios ARCH=arm/arm64")
endif


ifeq ($(OS),win32)
CFLAGS += -DWEBRTC_WIN
endif

ifeq ($(OS),macosx)
AR = libtool -static -o
LDFLAGS += -dynamiclib -lobjc -framework Foundation
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_MAC
DYEXT = dylib
endif

ifeq ($(OS),linux)
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_LINUX
endif

ifeq ($(OS),android)
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_LINUX -DWEBRTC_ANDROID
endif

ifeq ($(OS),ios)
CFLAGS += -DWEBRTC_POSIX -DWEBRTC_IOS
endif



ifeq ($(ARCH),arm)
CFLAGS += -DWEBRTC_ARCH_ARM_V7 -DWEBRTC_ARCH_ARM -DWEBRTC_HAS_NEON
endif

ifeq ($(ARCH),arm64)
CFLAGS += -DWEBRTC_ARCH_ARM64 -DWEBRTC_HAS_NEON
endif


# >use cmake to build:
#  cd third_party/crc32c/src/; mkdir build; cd build; cmake ..; make
LD_LIBS = third_party/crc32c/src/build/libcrc32c.a

# >use gen_lib.sh to build
#  cd third_party/abseil-cpp/libs/$(OS)/; sh gen_lib.sh
LD_LIBS += third_party/abseil-cpp/libs/$(OS)/libabsl_all.a


include common.mak
