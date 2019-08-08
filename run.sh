cmake_if=${1:-"false"}
cd build
if [ $cmake_if = "c" ]; then
    cmake ..
fi
make
cd ..
./bin/LoboFEM