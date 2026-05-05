# PROGRAM FLOW DEBUG 029

This package is intentionally rebased to the last known stable visual line: rpg024.

## Boot flow

1. `main()`
   - Sets palettes.
   - Calls `dialogue_init()`.
   - Calls `init_game()`.
   - Calls `load_map_mode()`.
   - Enters the game loop.

2. `init_game()`
   - Initializes player position and stats.
   - Sets `current_area = AREA_FIELD`.
   - Does not draw anything.

3. `load_map_mode()`
   - `DISPLAY_OFF`.
   - `SHOW_BKG`, `SHOW_SPRITES`, `HIDE_WIN`.
   - `snap_camera_to_player()`.
   - `draw_object_map()`.
   - `apply_camera_scroll()`.
   - `init_map_sprites()`.
   - `DISPLAY_ON`.

4. `draw_object_map()`
   - Builds a local static 32x32 BG buffer.
   - Draws object metatiles from the current map.
   - Applies the forest visual overlay.
   - Uploads BG tile graphics using:
     `set_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles)`
   - Uploads the 32x32 tilemap using:
     `set_bkg_tiles(0,0,32,32,bg)`.

5. `init_map_sprites()`
   - Loads player, NPC, and enemy sprite sheets.
   - Draws the existing actors.
   - This is the known-safe sprite path.

## White screen interpretation

A pure white screen usually means `DISPLAY_OFF` happened, but the program did not reach the final `DISPLAY_ON` in `load_map_mode()`.

Therefore, the primary suspects are:

- `draw_object_map()`
- `set_bkg_data()`
- `set_bkg_tiles()`
- `init_map_sprites()`
- ROM bank overflow / bad layout caused by code or data growth

## Why rpg025-rpg028 were rolled back

rpg025 added too many categories at once:

- soft mountain BG tiles
- `MAP_GFX_TILE_COUNT` 17 -> 21
- actor switching logic
- town wall changes
- field passability changes

Even after partial rollbacks, the safe path became muddy. rpg029 resets to rpg024-style behavior and adds documentation/comments only.
