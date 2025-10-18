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

#cp '/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0' .
#cp '/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0' .
#cp '/usr/lib/x86_64-linux-gnu/libSDL2_mixer-2.0.so.0' .
#cp '/usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so.0' .
#cp 'steamworks_sdk/sdk/redistributable_bin/linux64/libsteam_api.so' .
#
#zipname="exodus_steam-linux.zip"
#name="$(basename "$(pwd)")"
#
#pushd ..
#zip -r "${zipname}" "${name}" --include "${name}/assets/*" "${name}/exodus" "${name}/LICENSE" "${name}/manual.pdf" "${name}/README.md" "${name}/supplement.pdf" "${name}/libSDL2-2.0.so.0" "${name}/libSDL2_image-2.0.so.0" "${name}/libSDL2_mixer-2.0.so.0" "${name}/libSDL2_ttf-2.0.so.0" "${name}/libsteam_api.so" "${name}/licenses/*" "${name}/steam_appid.txt"
#mv "${zipname}" "${name}"
#popd

target_dir="steamworks_sdk/sdk/tools/ContentBuilder/content/linux"

rm -rf "$target_dir"
mkdir -p "$target_dir"

cp "exodus" "$target_dir"
cp "steam_appid.txt" "$target_dir"
cp -r "assets" "$target_dir"
cp '/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0' "$target_dir"
cp '/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0' "$target_dir"
cp '/usr/lib/x86_64-linux-gnu/libSDL2_mixer-2.0.so.0' "$target_dir"
cp '/usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so.0' "$target_dir"
cp 'steamworks_sdk/sdk/redistributable_bin/linux64/libsteam_api.so' "$target_dir"
cp "manual.pdf" "$target_dir"
cp "supplement.pdf" "$target_dir"
cp "README.md" "$target_dir"
cp "LICENSE" "$target_dir"
cp -r "licenses" "$target_dir"

cp steam/exodus_*.vdf "steamworks_sdk/sdk/tools/ContentBuilder/scripts"

echo ""
echo "Next steps:"
echo "  * Run build_steam-*.sh scripts for any other platforms required"
echo "  * Verify everything is present and correct under $target_dir"
echo "  * cd steamworks_sdk/sdk/tools/ContentBuilder/builder_linux"
echo "  * chmod +x steamcmd.sh"
echo "  * chmod +x linux32/steamcmd"
echo "  * ./steamcmd"
echo "  * At the Steam prompt:"
echo "    * login <YOUR STEAM USERNAME>"
echo "    * Enter your password"
echo "    * run_app_build ../scripts/exodus_build_4098020.vdf"
echo "  * Go to the Steamworks web frontend"
echo "  * Go to Steampipe > Builds"
echo "  * Set the build live on the desired branch"
