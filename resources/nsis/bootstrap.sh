#!/bin/sh

CMAKE_VERSION=3.11

mv /C/msys64/mingw64/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in /C/msys64/mingw64/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in.orig
mv /C/msys64/mingw32/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in /C/msys64/mingw32/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in.orig
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw64/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw32/share/cmake-$CMAKE_VERSION/Modules/NSIS.template.in
