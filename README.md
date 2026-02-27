# Windows Compile

地址：git clone https://gitee.com/foheart_1_0/IMULinkSDK_Install.git

## 编译Debug版本

cmake -S . -B out/build/x64-Debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug"

cmake --build out/build/x64-Debug --config Debug

## 编译Release版本

cmake -S . -B out/build/x64-Release -G "Visual Studio 17 2022" -A x64 -DCMAKE_CONFIGURATION_TYPES="Release"

cmake --build out/build/x64-Release --config Release

## 头文件

## 库文件

Debug版本

out\install\x64-debug\bin\imuLinkSDK.dll

out\install\x64-debug\lib\imuLinkSDK.lib

Release版本

out\install\x64-release\bin\imuLinkSDK.dll

out\install\x64-release\lib\imuLinkSDK.lib

## 中间文件

中间文件在out/build文件夹下，编译错误时可以删除out/build文件夹重新编译。
