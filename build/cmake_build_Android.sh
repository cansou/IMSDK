#!/usr/bin/env bash
this_dir=`pwd`

cd ..
mkdir Android
cd Android



cmake -D__ANDROID__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/android.toolchain.cmake -DANDROID_NDK=$NDK_ROOT -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi" ..


