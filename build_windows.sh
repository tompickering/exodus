#!/bin/bash

mkdir -p include/x86_64-linux-gnu
ln -sf /usr/include/SDL2 include
ln -sf /usr/include/x86_64-linux-gnu/SDL2 include/x86_64-linux-gnu

mkdir lib
pushd lib
wget -c https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-devel-2.26.5-mingw.tar.gz
tar -xvf SDL2-devel-2.26.5-mingw.tar.gz
cp SDL2-2.26.5/x86_64-w64-mingw32/lib/libSDL2.dll.a .
wget -c https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-mingw.tar.gz
tar -xvf SDL2_image-devel-2.6.3-mingw.tar.gz
cp SDL2_image-2.6.3/x86_64-w64-mingw32/lib/libSDL2_image.dll.a .
wget -c https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-devel-2.6.3-mingw.tar.gz
tar -xvf SDL2_mixer-devel-2.6.3-mingw.tar.gz
cp SDL2_mixer-2.6.3/x86_64-w64-mingw32/lib/libSDL2_mixer.dll.a .
wget -c https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-mingw.tar.gz
tar -xvf SDL2_ttf-devel-2.20.2-mingw.tar.gz
cp SDL2_ttf-2.20.2/x86_64-w64-mingw32/lib/libSDL2_ttf.dll.a .
popd

make clean
make -j7 windows
