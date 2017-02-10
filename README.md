
# Development

```
# Update your toolchain
pacman -Syuu

# Install main development packages
pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake pkg-config

# Install SDL2 development packages
pacman -S --noconfirm --needed mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer

# Install SDL_kitchensink dependencies
pacman -S --noconfirm --needed mingw-w64-x86_64-libass mingw-w64-x86_64-ffmpeg
```
