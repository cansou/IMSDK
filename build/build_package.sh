#!/usr/bin/env bash

################################################################################
# Print help information
################################################################################
print_help_and_exit(){
    print_info_in_red "${1}"
    echo "Usage: ${0} [ci] [svn] [no_recognize|ifly_recognize|usc_recognize|ali_recognize] [-h] [android] [ios] [macos]"
    echo "recognize    - 是否开启语音转文字识别"
    echo "-h           - help"
    echo "android      - 编译Android平台"
    echo "ios          - 编译iOS平台"
    echo "macos        - 编译macOS平台"
    echo "unity        - 编译Unity平台"
    exit 1
}

cd ..
WORKSPACE=`pwd`

recognize_type=0				# 0:所有识别	1:无识别	2:讯飞	3:云知声	4:阿里
android="no"
ios="no"
macos="no"
unity="no"

# Parse parameters
for var in $*
do
    if [ $var = "-h" ]; then
        print_help_and_exit
    elif [ $var = "ci" ]; then
        cibuild=1
    elif [ $var = "svn" ]; then
        svncommit=1
    elif [ $var = "no_recognize" ]; then
    	recognize_type=1
	elif [ $var = "ifly_recognize" ]; then
    	recognize_type=2
	elif [ $var = "usc_recognize" ]; then
    	recognize_type=3
	elif [ $var = "ali_recognize" ]; then
    	recognize_type=4	
    elif [ $var = "android" ]; then
        android="yes"
    elif [ $var = "ios" ]; then
        ios="yes"
    elif [ $var = "macos" ]; then
        macos="yes"
    elif [ $var = "unity" ]; then
        unity="yes"
    fi
done

if [ $recognize_type -eq 1 ]; then
    packagenameattach="_mini"
elif [ $recognize_type -eq 2 ]; then
    packagenameattach="_ifly"
elif [ $recognize_type -eq 3 ]; then
    packagenameattach="_usc"
elif [ $recognize_type -eq 4 ]; then
    packagenameattach="_al"
fi

#clean
rm -rf $WORKSPACE/Output
mkdir $WORKSPACE/Output

build_android(){
    #prepare
    chmod +x $WORKSPACE/build/cmake_build_Android.sh
    chmod +x $WORKSPACE/build/cmake_build_Android_v7a.sh
    chmod +x $WORKSPACE/build/cmake_build_Android_x86.sh
    chmod +x $WORKSPACE/build/cmake_build_Android_x86_64.sh
    chmod +x $WORKSPACE/build/cmake_build_Android_v8a.sh
    chmod +x $WORKSPACE/build/cmake_build_IOS64.sh
    chmod +x $WORKSPACE/build/cmake_build_IOS64Simulator.sh
    chmod +x $WORKSPACE/build/cmake_build_OSX64_dylib.sh

    #build android armeabi
    rm -rf $WORKSPACE/Android
    cd $WORKSPACE/build
    ./cmake_build_Android.sh
    cd $WORKSPACE/Android
    make -j4
    if [ $? -eq 0 ];then
        echo "Android armeabi library build success."
    else
        echo "Android armeabi library build failed."
        exit 1
    fi

    #build android armeabi-v7a
    rm -rf $WORKSPACE/Android
    cd $WORKSPACE/build/
    ./cmake_build_Android_v7a.sh
    cd $WORKSPACE/Android
    make -j4
    if [ $? -eq 0 ];then
        echo "Android armeabi-v7a library build success."
    else
        echo "Android armeabi-v7a library build failed."
        exit 1
    fi

    #build android x86
    rm -rf $WORKSPACE/Android
    cd $WORKSPACE/build/
    ./cmake_build_Android_x86.sh
    cd $WORKSPACE/Android
    make -j4
    if [ $? -eq 0 ];then
        echo "Android armeabi-x86 library build success."
    else
        echo "Android armeabi-x86 library build failed."
        exit 1
    fi

    #build android x86_64
    rm -rf $WORKSPACE/Android
    cd $WORKSPACE/build/
    ./cmake_build_Android_x86_64.sh
    cd $WORKSPACE/Android
    make -j4
    if [ $? -eq 0 ];then
        echo "Android armeabi-x86_64 library build success."
    else
        echo "Android armeabi-x86_64 library build failed."
        exit 1
    fi

    #build android armv8a
    rm -rf $WORKSPACE/Android
    cd $WORKSPACE/build/
    ./cmake_build_Android_v8a.sh
    cd $WORKSPACE/Android
    make -j4
    if [ $? -eq 0 ];then
        echo "Android arm64-v8a library build success."
    else
        echo "Android arm64-v8a library build failed."
        exit 1
    fi

    #strip symboles
    cd $WORKSPACE/Output
    mkdir -p symbolefiles/armeabi
    mkdir -p symbolefiles/armeabi-v7a
    mkdir -p symbolefiles/x86
    mkdir -p symbolefiles/x86_64
    mkdir -p symbolefiles/arm64-v8a
    cp $WORKSPACE/lib/Android/Release/armeabi/libyim.so symbolefiles/armeabi/
    cp $WORKSPACE/lib/Android/Release/armeabi-v7a/libyim.so symbolefiles/armeabi-v7a/
    cp $WORKSPACE/lib/Android/Release/x86/libyim.so symbolefiles/x86/
    cp $WORKSPACE/lib/Android/Release/x86_64/libyim.so symbolefiles/x86_64/
    cp $WORKSPACE/lib/Android/Release/arm64-v8a/libyim.so symbolefiles/arm64-v8a/

    zip -r symbolefiles_${mainVersion}${nextBuildNo}.zip symbolefiles

    $NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-strip \
    $WORKSPACE/lib/Android/Release/armeabi/libyim.so
    $NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-strip \
    $WORKSPACE/lib/Android/Release/armeabi-v7a/libyim.so
    $NDK_ROOT/toolchains/x86-4.9/prebuilt/darwin-x86_64/bin/i686-linux-android-strip \
    $WORKSPACE/lib/Android/Release/x86/libyim.so
    $NDK_ROOT/toolchains/x86_64-4.9/prebuilt/darwin-x86_64/bin/x86_64-linux-android-strip \
    $WORKSPACE/lib/Android/Release/x86_64/libyim.so
    $NDK_ROOT/toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64/bin/aarch64-linux-android-strip \
    $WORKSPACE/lib/Android/Release/arm64-v8a/libyim.so

    #build jar
    cd $WORKSPACE/src/YouMeIMEngine/Android/java
    rm -rf build.gradle
    if [ $recognize_type -eq 1 ]
    then
        cp build_no_recognize.gradle build.gradle
    elif [ $recognize_type -eq 2 ]
    then
        cp build_ifly_recognize.gradle build.gradle
    elif [ $recognize_type -eq 3 ]
    then
        cp build_usc_recognize.gradle build.gradle
    elif [ $recognize_type -eq 4 ]
    then
        cp build_ali_recognize.gradle build.gradle  
    else
        cp build_all_recognize.gradle build.gradle
    fi
    if [ $? -eq 0 ];then
        echo "Android jar build success."
    else
        echo "Android jar build failed."
        exit 1
    fi

    chmod +x build.sh
    cd $WORKSPACE/src/YouMeIMEngine/Android/javasdk
    ./build.sh
    if [ ! -f "./build/libs/javasdk.jar" ]; then
      echo "error: ./build/libs/javasdk.jar not exsit."
      exit 1
    fi

    ## generate android sdk package
    cd $WORKSPACE/Output
    mkdir -p im_android/lib/armeabi
    mkdir -p im_android/lib/armeabi-v7a
    mkdir -p im_android/lib/arm64-v8a
    mkdir -p im_android/lib/x86
    mkdir -p im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/javasdk/build/libs/javasdk.jar im_android/lib/yim-sdk.jar
    cp $WORKSPACE/src/YouMeIMEngine/Android/javasdk/libs/gson-2.3.1.jar im_android/lib/
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/build/libs/java.jar im_android/lib/yim.jar
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/android-support-v4.jar im_android/lib/

    if [ $recognize_type -eq 0 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libuscasr.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuscasr.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuscasr.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuscasr.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuscasr.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/usc.jar im_android/lib/
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libnlscppsdk.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libnlscppsdk.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libnlscppsdk.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libnlscppsdk.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libnlscppsdk.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libuuid.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuuid.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuuid.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuuid.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuuid.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/alisr.jar im_android/lib/
    elif [ $recognize_type -eq 2 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/Msc.jar im_android/lib/
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/Sunflower.jar im_android/lib/
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libmsc.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libmsc.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libmsc.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libmsc.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libmsc.so im_android/lib/x86_64
    elif [ $recognize_type -eq 3 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libuscasr.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuscasr.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuscasr.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuscasr.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuscasr.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/usc.jar im_android/lib/
    elif [ $recognize_type -eq 4 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libnlscppsdk.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libnlscppsdk.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libnlscppsdk.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libnlscppsdk.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libnlscppsdk.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi/libuuid.so im_android/lib/armeabi
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuuid.so im_android/lib/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuuid.so im_android/lib/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuuid.so im_android/lib/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuuid.so im_android/lib/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/alisr.jar im_android/lib/
    fi

    cp $WORKSPACE/lib/Android/Release/armeabi/libyim.so im_android/lib/armeabi/libyim.so
    cp $WORKSPACE/lib/Android/Release/armeabi-v7a/libyim.so im_android/lib/armeabi-v7a/libyim.so
    cp $WORKSPACE/lib/Android/Release/arm64-v8a/libyim.so im_android/lib/arm64-v8a/libyim.so
    cp $WORKSPACE/lib/Android/Release/x86/libyim.so im_android/lib/x86/libyim.so
    cp $WORKSPACE/lib/Android/Release/x86_64/libyim.so im_android/lib/x86_64/libyim.so
    #cp $WORKSPACE/ReleaseNote.txt youme_im_engine_android/

    zip -r im_android_${mainVersion}${buildNo}${packagenameattach}.zip im_android
}
    
build_ios(){
    #gernerate ios project
    rm -rf $WORKSPACE/IOS
    cd $WORKSPACE/build
    if [ $recognize_type -eq 1 ]
    then
        ./cmake_build_IOS64.sh no_recognize
    elif [ $recognize_type -eq 2 ]
    then
        ./cmake_build_IOS64.sh ifly_recognize
    elif [ $recognize_type -eq 3 ]
    then
        ./cmake_build_IOS64.sh usc_recognize
    elif [ $recognize_type -eq 4 ]
    then
        ./cmake_build_IOS64.sh ali_recognize        
    else
        ./cmake_build_IOS64.sh
    fi

    rm -rf $WORKSPACE/IOS_Simulator
    cd $WORKSPACE/build
    if [ $recognize_type -eq 1 ]
    then
        ./cmake_build_IOS64Simulator.sh no_recognize
    elif [ $recognize_type -eq 2 ]
    then
        ./cmake_build_IOS64Simulator.sh ifly_recognize
    elif [ $recognize_type -eq 3 ]
    then
        ./cmake_build_IOS64Simulator.sh usc_recognize
    elif [ $recognize_type -eq 4 ]
    then
        ./cmake_build_IOS64Simulator.sh ali_recognize       
    else
        ./cmake_build_IOS64Simulator.sh
    fi

    #build ios
    cd $WORKSPACE/IOS
    xcodebuild clean -project $WORKSPACE/IOS/YouMeIM.xcodeproj || echo "clean fail,maybe ok."
    xcodebuild -project YouMeIM.xcodeproj -target yim -configuration Release -sdk iphoneos \
    -arch armv7 -arch arm64
    if [ $? -eq 0 ];then
        echo "iOS device library build success."
    else
        echo "iOS simulator library build failed."
        exit 1
    fi

    #build ios simulator
    cd $WORKSPACE/IOS_Simulator
    xcodebuild clean -project YouMeIM.xcodeproj || echo "clean fail,maybe ok."
    xcodebuild -project YouMeIM.xcodeproj -target yim -configuration Release \
    -sdk iphonesimulator -arch i386 -arch x86_64
    if [ $? -eq 0 ];then
        echo "iOS simulator library build success."
    else
        echo "iOS simulator library build failed."
        exit 1
    fi

    ## generate iOS sdk package
    cd $WORKSPACE/Output
    mkdir -p im_ios/lib
    mkdir -p im_ios/include
    cp $WORKSPACE/src/YouMeIMEngine/YIMPlatformDefine.h im_ios/include
    cp $WORKSPACE/src/YouMeIMEngine/YIM.h im_ios/include
    cp $WORKSPACE/build/thirdLib/YouMeiOSSDK/* im_ios/include
    cp $WORKSPACE/youme_common/lib/ios/libYouMeCommon.a im_ios/lib
    lipo -create $WORKSPACE/lib/IOS/Release/libyim.a $WORKSPACE/lib/IOSSIMULATOR/Release/libyim.a \
    -output im_ios/lib/libyim.a

    if [ $recognize_type -eq 0 ]
    then
    # cp -r $WORKSPACE/build/thirdLib/iflyMSC.framework im_ios/lib/
    cp -r $WORKSPACE/build/thirdLib/USCModule.framework im_ios/lib/
    cp -r $WORKSPACE/build/thirdLib/AliyunNlsSdk.framework im_ios/lib/
    elif [ $recognize_type -eq 2 ]
    then
    cp -r $WORKSPACE/build/thirdLib/iflyMSC.framework im_ios/lib/
    elif [ $recognize_type -eq 3 ]
    then
    cp -r $WORKSPACE/build/thirdLib/USCModule.framework im_ios/lib/
    elif [ $recognize_type -eq 4 ]
    then
    cp -r $WORKSPACE/build/thirdLib/AliyunNlsSdk.framework im_ios/lib/
    fi
    #cp $WORKSPACE/ReleaseNote.txt im_ios/

    zip -r im_ios_${mainVersion}${buildNo}${packagenameattach}.zip im_ios
}

build_macos(){
    #gernerate mac project
    rm -rf $WORKSPACE/OSX
    cd $WORKSPACE/build
    ./cmake_build_OSX64_dylib.sh

    cd $WORKSPACE/OSX
    xcodebuild clean -project YouMeIM.xcodeproj || echo "clean fail,maybe ok."
    xcodebuild -project YouMeIM.xcodeproj -target yim -configuration Release \
    -sdk macosx -arch x86_64
    if [ $? -eq 0 ];then
    echo "OSX library build success."
    else
    echo "OSX library build failed."
    exit 1
    fi

    ## generate MAC sdk package
    cd $WORKSPACE/Output
    mkdir -p im_osx/lib

    cp $WORKSPACE/lib/MAC/Release/libyim.dylib im_osx/lib
    zip -r im_osx_${mainVersion}${buildNo}${packagenameattach}.zip im_osx
}

build_unity(){
    PROJ_PATH="$WORKSPACE/IMDemo/U3DWrapper"
    #for unity
    #cp $WORKSPACE/lib/Android/Release/armeabi/libyim.so $WORKSPACE/U3DWrapper/Assets/Plugins/Android/libs/armeabi/libyim.so
    mkdir -p $WORKSPACE/U3DWrapper/Assets/Plugins/Android/libs/arm64-v8a/
    cp $WORKSPACE/lib/Android/Release/arm64-v8a/libyim.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a/libyim.so
    mkdir -p $WORKSPACE/U3DWrapper/Assets/Plugins/Android/libs/armeabi-v7a/
    cp $WORKSPACE/lib/Android/Release/armeabi-v7a/libyim.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a/libyim.so
    mkdir -p $WORKSPACE/U3DWrapper/Assets/Plugins/Android/libs/x86/
    cp $WORKSPACE/lib/Android/Release/x86/libyim.so $PROJ_PATH/Assets/Plugins/Android/libs/x86/libyim.so
    mkdir -p $WORKSPACE/U3DWrapper/Assets/Plugins/Android/libs/x86_64/
    cp $WORKSPACE/lib/Android/Release/x86_64/libyim.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64/libyim.so
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/build/libs/java.jar $PROJ_PATH/Assets/Plugins/Android/yim.jar
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/android-support-v4.jar $PROJ_PATH/Assets/Plugins/Android/
    cp $WORKSPACE/Output/im_ios/lib/libYouMeCommon.a $PROJ_PATH/Assets/Plugins/iOS/libYouMeCommon.a
    cp $WORKSPACE/Output/im_ios/lib/libyim.a $PROJ_PATH/Assets/Plugins/iOS/libyim.a

    # macos lib
    cp $WORKSPACE/lib/MAC/Release/libyim.dylib $PROJ_PATH/Assets/Plugins/
    cd $WORKSPACE/U3DWrapper/Assets/Plugins/
    mv libyim.dylib yim.bundle

    if [ $recognize_type -eq 0 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/usc.jar $PROJ_PATH/Assets/Plugins/Android
    cp -r $WORKSPACE/build/thirdLib/USCModule.framework $PROJ_PATH/Assets/Plugins/iOS/
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/alisr.jar $PROJ_PATH/Assets/Plugins/Android
    cp -r $WORKSPACE/build/thirdLib/AliyunNlsSdk.framework $PROJ_PATH/Assets/Plugins/iOS/
    elif [ $recognize_type -eq 2 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libmsc.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libmsc.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libmsc.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libmsc.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/Msc.jar $PROJ_PATH/Assets/Plugins/Android
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/Sunflower.jar $PROJ_PATH/Assets/Plugins/Android
    cp -r $WORKSPACE/build/thirdLib/iflyMSC.framework $PROJ_PATH/Assets/Plugins/iOS/
    elif [ $recognize_type -eq 3 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuscasr.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/usc.jar $PROJ_PATH/Assets/Plugins/Android
    cp -r $WORKSPACE/build/thirdLib/USCModule.framework $PROJ_PATH/Assets/Plugins/iOS/
    elif [ $recognize_type -eq 4 ]
    then
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libnlscppsdk.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/armeabi-v7a/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/armeabi-v7a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/arm64-v8a/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/arm64-v8a
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/x86
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/x86_64/libuuid.so $PROJ_PATH/Assets/Plugins/Android/libs/x86_64
    cp $WORKSPACE/src/YouMeIMEngine/Android/java/libs/alisr.jar $PROJ_PATH/Assets/Plugins/Android
    cp -r $WORKSPACE/build/thirdLib/AliyunNlsSdk.framework $PROJ_PATH/Assets/Plugins/iOS/
    fi

    # Unity 还不支持x86_64， 导出插件时先移除掉 x86_64的库
    rm -rf $PROJ_PATH/Assets/Plugins/Android/libs/x86_64

    UNITY="/Applications/Unity/Unity.app/Contents/MacOS/Unity"
    EXPORT_FILE="$WORKSPACE/Output/im_unity_${mainVersion}${buildNo}.unitypackage"
    ${UNITY} -batchmode -quit -projectPath "${PROJ_PATH}" -executeMethod AutoSdkExport.AutoExport \
    "Assets/Plugins" "Assets/YIMEngine" "Assets/YouMe" \
     "Assets/YIMTest.cs" "Assets/YIMTest.unity" "${EXPORT_FILE}"
    if [ $? -eq 0 ];then
     echo "unity package export success."
    else
     echo "unity package export fail."
     exit 1
    fi

    cd "$WORKSPACE/Output/"
    zip -r im_unity_${mainVersion}${buildNo}${packagenameattach}.zip im_unity_${mainVersion}${buildNo}.unitypackage
}

if [ $android = "yes" ];then
    build_android
fi

if [ $ios = "yes" ];then
    build_ios
fi

if [ $macos = "yes" ];then
    build_macos
fi

if [ $unity = "yes" ];then
    build_unity
fi

echo "build success. finished."
