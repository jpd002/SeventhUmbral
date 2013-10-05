export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
mkdir -p build
cd build
cmake ..
make install
cd ..
