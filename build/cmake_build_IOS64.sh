#!/usr/bin/env bash
this_dir=`pwd`

export PATH=$this_dir/cmake/mac/bin/CMake.app/Contents/bin:$PATH
echo $PATH
cd ..
mkdir IOS
cd IOS

if [ $# -ge 1 ] && [ $1 = no_recognize ]
then
   cmake -D__IOS__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..
elif [ $# -ge 1 ] && [ $1 = ifly_recognize ] 
then
   cmake -D__IOS__=1 -D__IFLY_RECOGNIZE__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..
elif [ $# -ge 1 ] && [ $1 = usc_recognize ] 
then
   cmake -D__IOS__=1 -D__USC_RECOGNIZE__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..
elif [ $# -ge 1 ] && [ $1 = ali_recognize ] 
then
   cmake -D__IOS__=1 -D__ALI_RECOGNIZE__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..   
else
   cmake -D__IOS__=1 -D__IFLY_RECOGNIZE__=0 -D__USC_RECOGNIZE__=1 -D__ALI_RECOGNIZE__=1 -DCMAKE_TOOLCHAIN_FILE=./build/toolchain/ios.cmake -GXcode ..    
fi



