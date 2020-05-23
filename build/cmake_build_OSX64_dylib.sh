#!/usr/bin/env bash
this_dir=`pwd`

export PATH=$this_dir/cmake/mac/bin/CMake.app/Contents/bin:$PATH
echo $PATH
cd ..
mkdir OSX
cd OSX

cmake -D__OSX_DYLIB__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/macos.cmake .. -G"Xcode"

#cmake -D__OSX_DYLIB__=1 .. -G"Xcode"

