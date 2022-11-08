
gen_cmake() {
    cmake ../.. -GXcode \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=9 \
        -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
        -DCMAKE_SYSTEM_PROCESSOR="arm64" \
        -DCMAKE_TRY_COMPILE_TARGET_TYPE="STATIC_LIBRARY" \
        -DCMAKE_CXX_STANDARD=11
}

gen_make() {
    #make
    xcodebuild -project absl.xcodeproj -target ALL_BUILD -sdk iphoneos -configuration Release OTHER_CFLAGS="-fembed-bitcode" OTHER_CPLUSPLUSFLAGS="-fembed-bitcode" CLANG_CXX_LANGUAGE_STANDARD="c++0x" CLANG_CXX_LIBRARY="libc++" ENABLE_BITCODE=YES BITCODE_GENERATION_MODE="marker"

    # no valid for bitcode
    #xcodebuild -project absl.xcodeproj -target ALL_BUILD -sdk iphoneos -configuration Release
    #xcodebuild -project absl.xcodeproj -target ALL_BUILD -sdk iphonesimulator -configuration Release
}

gen_lib() {
    libs=$(find absl -name lib\*.a | grep -v "Objects-normal")
    echo $libs
    echo "> packaging ..."
    libtool -static $libs -o libabsl_all.a
}


gen_$1
