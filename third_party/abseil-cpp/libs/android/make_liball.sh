#!/bin/bash
# author: peterxu
#

## build config
ROOT=`pwd`
HOST=`uname`

check_err() {
    [ $? != 0 ] && echo "[*] Error and exit!!!, reason=$1" && exit 1
}


make_archive () {
    osname="$1"
    target="$2"
    thelibs="$3"
    echo "[-] Generating archive $target by $AR ..."
    if [ $osname = "UNIX" ] || [ $osname = "ANDROID" ]; then
        TMP="$ROOT/.tmpobj"
        rm -rf $TMP
        mkdir -p $TMP
        cd $TMP
        for item in $thelibs; do
            lib=../$item
            if [ ! -e $lib ]; then
                echo "Can not find $lib!"; continue
            else
                #echo "Processing $lib ..."
                $AR x $lib
            fi
        done
        ls *.o | xargs $AR qc $target
        echo "Adding symbol table to archive."
        $AR sv $target
        mv $target $ROOT
        cd $ROOT
        rm -rf $TMP
    elif [ $osname = "IOS" ]; then
        libtool -static -arch_only armv7 -o lib$target.a ${thelibs[@]:0}
    elif [ $osname = "IOS-SIM" ]; then
        libtool -static -arch_only i386 -o lib$target.a ${thelibs[@]:0}
    elif [ $osname = "MAC" ]; then
        libtool -static -arch_only x86_64 -o lib$target.a ${thelibs[@]:0}
    else
        usage
    fi
}


make_so () {
    osname="$1"
    target="$2"
    thelibs="$3"
    echo "[-] Generate shared $target ..."
    if [ $osname = "UNIX" ] || [ $osname = "ANDROID" ]; then
        $CC -shared -o $target -Wl,-whole-archive $thelibs -Wl,-no-whole-archive $ldflags
    elif [ $osname = "MAC" ]; then
        libtool -dynamic -arch_only x86_64 -o $target ${thelibs[@]:0} $ldflags
    fi
}

usage() {
    echo "usage: $0 IOS|IOS-SIM|ANDROID target libpath1 [libpath2 ...]"
}

main() {
    [ $# -le 2 ] && usage && return

    targetos=$1 && shift
    targetlib="lib$1.a" && shift

    sublibs=""
    for path in $*; do
        sublibs="$sublibs `find $path -name "lib*.a" -print`"
    done
    #echo $sublibs

    # for static lib
    rm -f $targetlib
    make_archive "$targetos" "$targetlib" "$sublibs" 2>/dev/null
    check_err "fail to gen archive $targetlib"
}

main $*
exit 0
