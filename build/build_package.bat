set WORKSPACE="%cd%/.."
node ci_tool_upgradeversion.js


cd %WORKSPACE%
rmdir /s /q Win32
rmdir /s /q bin
cd build
call cmake_build_Win32_ci.bat

cd %WORKSPACE%
cd Win32
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com" YouMeIM.sln /build "Release|Win32" /project src\YouMeIMEngine\yim.vcxproj || exit /b 1
cd %WORKSPACE%
cd bin\Release
ren yim.dll yim_win32.dll
pushd d:\DigitalSignature
digsign.py %WORKSPACE%\bin\Release\yim_win32.dll || exit /b 1
popd


d:
cd %WORKSPACE%
rmdir /s /q Win64
cd build
call cmake_build_Win64_ci.bat

d:
cd %WORKSPACE%
cd Win64
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com" YouMeIM.sln /build "Release|x64" /project src\YouMeIMEngine\yim.vcxproj || exit /b 1

pushd d:\DigitalSignature
digsign.py %WORKSPACE%\bin\Release\yim.dll || exit /b 1
popd


d:
cd %WORKSPACE%
rmdir /s /q Win32
cd build
call cmake_build_Win32_ci_debug.bat
d:
cd %WORKSPACE%
cd Win32
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com" YouMeIM.sln /build "Debug|Win32" /project src\YouMeIMEngine\yim_debug.vcxproj || exit /b 1
cd %WORKSPACE%
cd bin\Debug
ren yim_debug.dll yim_debug_win32.dll
pushd d:\DigitalSignature
digsign.py %WORKSPACE%\bin\Debug\yim_debug_win32.dll || exit /b 1
popd


d:
cd %WORKSPACE%
rmdir /s /q Win64
cd build
call cmake_build_Win64_ci_debug.bat

d:
cd %WORKSPACE%
cd Win64
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\IDE\devenv.com" YouMeIM.sln /build "Debug|x64" /project src\YouMeIMEngine\yim_debug.vcxproj || exit /b 1

pushd d:\DigitalSignature
digsign.py %WORKSPACE%\bin\Debug\yim_debug.dll ||exit /b 1
popd
