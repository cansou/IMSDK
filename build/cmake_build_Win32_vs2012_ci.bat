@echo off

set PATH=%PATH%;../build/cmake/windows/bin
cd ..
mkdir Win32
cd Win32
cmake .. -G"Visual Studio 11 2012" -DCMAKE_CONFIGURATION_TYPES=Debug;Release

