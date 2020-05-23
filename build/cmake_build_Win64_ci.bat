@echo off

set PATH=%PATH%;../build/cmake/windows/bin
cd ..
mkdir Win64
cd Win64
cmake .. -G"Visual Studio 12 2013 Win64" -DCMAKE_GENERATOR_TOOLSET=v120_xp -DWIN64=1 -DCMAKE_CONFIGURATION_TYPES=Debug;Release

