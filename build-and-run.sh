bun run scripts/build_objs.ts

cmake -B _wii -D CMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Wii.cmake
cmake --build _wii

if [ $? -ne 0 ]; then
    echo "Build failed..."
    exit 1
fi
 
dolphin-emu _wii/application.dol