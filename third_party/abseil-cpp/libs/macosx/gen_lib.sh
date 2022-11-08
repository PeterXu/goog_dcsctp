
gen_cmake() {
    cmake ../.. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=11
}

gen_make() {
    make
}

gen_lib() {
    libs=$(find absl -name lib\*.a)
    echo $libs
    libtool -static $libs -o libabsl_all.a
}


gen_$1
