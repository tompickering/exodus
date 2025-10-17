#!/bin/bash

git diff --exit-code >/dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "There are uncommitted changes; please resolve these before running"
    exit
fi

echo "PREREQUISITE STEPS:"
echo "Download the Steam SDK from https://partner.steamgames.com/downloads/list"
echo "*** CURRENTLY EXPECTS v1.62 ***"
echo "mkdir steamworks_sdk"
echo "cd steamworks_sdk"
echo "unzip ~/Downloads/steamworks_sdk_xxx.zip"
echo

if [[ ! -d "steamworks_sdk" ]]; then
    echo "Error: steamworks_sdk directory not found"
    exit
fi

make clean
make -j7 steam-linux

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit
fi

cp '/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0' .
cp '/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0' .
cp '/usr/lib/x86_64-linux-gnu/libSDL2_mixer-2.0.so.0' .
cp '/usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so.0' .
cp 'steamworks_sdk/sdk/redistributable_bin/linux64/libsteam_api.so' .

zipname="exodus_steam-linux.zip"
name="$(basename "$(pwd)")"

pushd ..
zip -r "${zipname}" "${name}" --include "${name}/assets/*" "${name}/exodus" "${name}/LICENSE" "${name}/manual.pdf" "${name}/README.md" "${name}/supplement.pdf" "${name}/*.so.0" "${name}/libsteam_api.so" "${name}/licenses/*" "${name}/steam_appid.txt"
mv "${zipname}" "${name}"
popd
