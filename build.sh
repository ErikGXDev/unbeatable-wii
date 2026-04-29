bun run scripts/build_objs.ts

cmake -B _wii -D CMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Wii.cmake
cmake --build _wii