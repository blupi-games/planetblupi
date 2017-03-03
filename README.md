
# Development with Linux or Darwin

## Build

```
mkdir Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

mkdir Release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

# Development with MSYS2 (Windows)

**Update your toolchain**
```
pacman -Syuu
```

**Install main development packages**
```
pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake pkg-config
```

## Build

```
mkdir Debug
cmake -G"MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

mkdir Release
cmake -G"MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
