# Pokémon World — 3DS prototype starter

This is a tiny original homebrew proof-of-concept for the planned 3DS game. It is **not** the full Pokémon game and does not include copyrighted Pokémon artwork, names, music, maps, or story assets.

## What this prototype tests

- Nintendo 3DS homebrew startup using `libctru`
- D-pad movement on a small text-map
- Seven region labels as a navigation placeholder
- Basic save/load of player coordinates and selected region to `sdmc:/pokemon_world_save.dat`

Controls:

- D-pad: move
- A: save
- B: load
- L/R: cycle through the seven region placeholders
- START: exit

## Build

Build on a computer with the devkitPro 3DS development environment installed (`devkitARM`, `libctru`, and the 3DS build rules):

```sh
make
```

The build should produce `pokemon_world_test.3dsx` (and related build output) when the toolchain is correctly installed.

## Important status

The source has been written as a starter, but it has **not been compiled or tested in a 3DS emulator in this environment** because the devkitARM compiler/toolchain is not installed here. Do not treat it as a verified working `.3dsx` yet. The next milestone is to compile it, test controls and save/load in an emulator, fix any issues, then replace the text map with original 2D pixel-art tiles and sprites.
