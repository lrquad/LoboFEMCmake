cmake_if=${1:-"false"}

if [ $cmake_if = "r" ]; then
    rm -r build/
    mkdir build
fi

cd build

if [ "$cmake_if" = "r" ] || [ "$cmake_if" = "c" ]; then
    cmake ..
fi

make
cd ..
catchsegv ./bin/LoboFEM -o hai