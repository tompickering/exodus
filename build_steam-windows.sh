#!/bin/bash


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

echo "N.B. Currently, a line needs to be manually replaced in the Makefile before running this build - is that done?"
sleep 3

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
make -j7 steam-windows

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit
fi

#cp lib/SDL2-2.26.5/x86_64-w64-mingw32/bin/SDL2.dll .
#cp lib/SDL2_image-2.6.3/x86_64-w64-mingw32/bin/SDL2_image.dll .
#cp lib/SDL2_mixer-2.6.3/x86_64-w64-mingw32/bin/SDL2_mixer.dll .
#cp lib/SDL2_ttf-2.20.2/x86_64-w64-mingw32/bin/SDL2_ttf.dll .
#cp steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.dll .
#
#zipname="exodus_steam-windows.zip"
#name="$(basename "$(pwd)")"
#
#pushd ..
#zip -r "${zipname}" "${name}" --include "${name}/assets/*" "${name}/exodus.exe" "${name}/LICENSE" "${name}/manual.pdf" "${name}/README.md" "${name}/supplement.pdf" "${name}/SDL2.dll" "${name}/SDL2_image.dll" "${name}/SDL2_mixer.dll" "${name}/SDL2_ttf.dll" "${name}/steam_api64.dll" "${name}/licenses/*" "${name}/steam_appid.txt"
#mv "${zipname}" "${name}"
#popd

target_dir="steamworks_sdk/sdk/tools/ContentBuilder/content/windows"

rm -rf "$target_dir"
mkdir -p "$target_dir"

cp "exodus.exe" "$target_dir"
cp "steam_appid.txt" "$target_dir"
cp -r "assets" "$target_dir"
cp 'lib/SDL2-2.26.5/x86_64-w64-mingw32/bin/SDL2.dll' "$target_dir"
cp 'lib/SDL2_image-2.6.3/x86_64-w64-mingw32/bin/SDL2_image.dll' "$target_dir"
cp 'lib/SDL2_mixer-2.6.3/x86_64-w64-mingw32/bin/SDL2_mixer.dll' "$target_dir"
cp 'lib/SDL2_ttf-2.20.2/x86_64-w64-mingw32/bin/SDL2_ttf.dll' "$target_dir"
cp 'steamworks_sdk/sdk/redistributable_bin/win64/steam_api64.dll' "$target_dir"
cp "manual.pdf" "$target_dir"
cp "supplement.pdf" "$target_dir"
cp "README.md" "$target_dir"
cp "LICENSE" "$target_dir"
cp -r "licenses" "$target_dir"

exodus_build='''
"AppBuild"
{
    "AppID" "4098020"
    "Desc" "Windows"
    "Preview" "0"
    "ContentRoot" "../content/"
    "verbose" "1"
    "Depots"
    {
        "4098022" "exodus_depot_build_4098022.vdf"
    }
}
'''

depot_build='''
"DepotBuild"
{
    "DepotID" "4098022"

    "FileMapping"
    {
        "LocalPath" "./windows/*"
        "DepotPath" "."
        "Recursive" "1"
    }
}
'''

echo "$exodus_build" > "steamworks_sdk/sdk/tools/ContentBuilder/scripts/exodus_build_windows_4098020.vdf"
echo "$depot_build" > "steamworks_sdk/sdk/tools/ContentBuilder/scripts/exodus_depot_build_4098022.vdf"

echo ""
echo "Next steps:"
echo "  * Verify everything is present and correct under $target_dir"
echo "  * cd steamworks_sdk/sdk/tools/ContentBuilder/builder_linux"
echo "  * chmod +x steamcmd.sh"
echo "  * chmod +x linux32/steamcmd"
echo "  * ./steamcmd"
echo "  * At the Steam prompt:"
echo "    * login <YOUR STEAM USERNAME>"
echo "    * Enter your password"
echo "    * run_app_build ../scripts/exodus_build_windows_4098020.vdf"
echo "  * Go to the Steamworks web frontend"
echo "  * Go to Steampipe > Builds"
echo "  * Set the build live on the desired branch"
