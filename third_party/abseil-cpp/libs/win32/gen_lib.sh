bpath="../../build"
mkdir -p $bpath
cd $bpath

cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j 4

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j 4
