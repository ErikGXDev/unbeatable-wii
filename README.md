# Unbiitable

A Wii port of UNBEATABLE.

## Requirements

- cmake
- DevKitPro + the wii-dev package group (figure this out yourself)
- GRRLIB, install the modified version from `vendored/grrlib` according to its README instructions, it contains fixes for aspect ratio
- Bun, for running compile-time scripts in `scripts/`
- (Optional) Dolphin emulator (dolphin-emu cli) for directly running the compiled game

## Usage

You can use the `build` and `build-and-run` scripts to quickly build (and run) the game.

## Building 3D stuff

(Using a batch exporter extension for blender is recommended. Use it to quickly export multiple objects to .obj)

`bun scripts/build_objs.ts` will scan for any .obj (+.mtl) file in assets/\* and convert all vertex and texture data into constants. Textures will automatically be copied to the `data` folder, which is bundled into the game.
Objects that contain the name "marker" will only store position data.
Output is at `source/generated/exported.h`.
