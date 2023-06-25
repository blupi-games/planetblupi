#!/bin/sh

CMAKE_VERSION=3.15

mv /C/msys64/mingw64/share/cmake/Modules/Internal/CPack/NSIS.template.in /C/msys64/mingw64/share/cmake/Modules/Internal/CPack/NSIS.template.in.orig
mv /C/msys64/mingw32/share/cmake/Modules/Internal/CPack/NSIS.template.in /C/msys64/mingw32/share/cmake/Modules/Internal/CPack/NSIS.template.in.orig
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw64/share/cmake/Modules/Internal/CPack/NSIS.template.in
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw32/share/cmake/Modules/Internal/CPack/NSIS.template.in
