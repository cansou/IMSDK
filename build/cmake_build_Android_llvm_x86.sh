#!/usr/bin/env bash
this_dir=`pwd`

export PATH=$this_dir/cmake/mac/bin/CMake.app/Contents/bin:$PATH
echo $PATH
cd ..
mkdir AndroidX86
cd AndroidX86



cmake -D__ANDROID__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/android.toolchain.cmake -DANDROID_NDK=$NDK_ROOT -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="x86" -DANDROID_STL=c++_static ..


