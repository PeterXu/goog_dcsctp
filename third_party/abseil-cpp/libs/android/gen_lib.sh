abis="arm64-v8a armeabi-v7a x86_64 x86"

gen_cmake() {
    for abi in $abis; do
    cmake ../.. -B$abi \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=Android \
        -DCMAKE_ANDROID_NDK="/usr/local/android-ndk" \
        -DCMAKE_ANDROID_STL_TYPE="c++_static" \
        -DCMAKE_ANDROID_ARCH_ABI="$abi" \
        -DCMAKE_CXX_STANDARD=11
    done
}

gen_make() {
    for abi in $abis; do
        make -C $abi
    done
}

TMPDIR=/tmp/ndk-which-$(date +%s)
MYNDKDIR="$ANDROID_NDK"

# 'get_build_var_for_abi' was copied from ndk-gdb
get_build_var_for_abi () {
    if [ -z "$GNUMAKE" ] ; then
        GNUMAKE=make
    fi
    NDK_PROJECT_PATH=$TMPDIR $GNUMAKE --no-print-dir -f $MYNDKDIR/build/core/build-local.mk DUMP_$1 APP_ABI=$2
}

ndk_which() {
    WHICH=$1
    # create a temporary skeleton project so that we can leverage build-local.mk
    mkdir -p $TMPDIR/jni
    cat >$TMPDIR/jni/Android.mk << "END_OF_FILE"
include $(CLEAR_VARS)
END_OF_FILE

    ABI=$2
    TOOLCHAIN_PREFIX=`get_build_var_for_abi TOOLCHAIN_PREFIX $ABI`
    rm -Rf $TMPDIR

    # fully qualified file name
    FQFN=${TOOLCHAIN_PREFIX}$WHICH

    # use the host system's 'which' to decide/report if the file exists or not, and is executable
    which "$FQFN"
}

gen_lib() {
    for abi in $abis; do
        echo $abi
        export AR=$(ndk_which "ar" "$abi")
        bash make_liball.sh ANDROID absl_all $abi
        mv libabsl_all.a $abi
    done
}


gen_$1
