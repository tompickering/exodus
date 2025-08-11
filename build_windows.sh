#!/bin/bash

git diff --exit-code >/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "There are uncommitted changes; please resolve these before running"
    exit
fi

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

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit
fi

cp lib/SDL2-2.26.5/x86_64-w64-mingw32/bin/SDL2.dll .
cp lib/SDL2_image-2.6.3/x86_64-w64-mingw32/bin/SDL2_image.dll .
cp lib/SDL2_mixer-2.6.3/x86_64-w64-mingw32/bin/SDL2_mixer.dll .
cp lib/SDL2_ttf-2.20.2/x86_64-w64-mingw32/bin/SDL2_ttf.dll .

name=$(basename "$(pwd)")

pushd ..
zip -r exodus.zip "${name}" --include "${name}/assets/*" "${name}/exodus.exe" "${name}/LICENSE" "${name}/manual.pdf" "${name}/README.md" "${name}/supplement.pdf" "${name}/*.dll"
mv exodus.zip "${name}"
popd
