#!/bin/bash --login
path=$(pwd)
cd "$path"

cd ../java
rm -rf build
gradle jarRelease

echo "cd to $path"
cd "$path"
rm -rf build
gradle jarRelease

# if [ -n "$target" ] && [ "$target" = "msdk" ]; then
# ../gradle/bin/gradle -b build_msdk.gradle jarRelease
# fi

# if [ -z "$target" ] || [ "$target" = "nonemsdk" ]; then
# ../gradle/bin/gradle jarRelease
# fi

#cp build/libs/jdkplugin.jar ../../../../Plugins/Android/jdk-native.jar
