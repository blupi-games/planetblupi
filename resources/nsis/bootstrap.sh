#!/bin/sh

mv /C/msys64/mingw64/share/cmake-3.9/Modules/NSIS.template.in /C/msys64/mingw64/share/cmake-3.9/Modules/NSIS.template.in.orig
mv /C/msys64/mingw32/share/cmake-3.9/Modules/NSIS.template.in /C/msys64/mingw32/share/cmake-3.9/Modules/NSIS.template.in.orig
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw64/share/cmake-3.9/Modules/NSIS.template.in
cp $(dirname $0)/NSIS.template.in /C/msys64/mingw32/share/cmake-3.9/Modules/NSIS.template.in
