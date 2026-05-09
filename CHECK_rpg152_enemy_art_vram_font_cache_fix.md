# rpg152_enemy_art_vram_font_cache_fix

## Purpose
Fixes two rpg150/rpg151 enemy-art issues.

1. L-size golem was loaded into BG tile numbers 144-191, which collide with the Japanese font cache range 128-223. When battle text was drawn, glyph tiles overwrote the golem tiles, so text-like shapes appeared on the golem body.
2. `battle_copy_enemy_from_data()` still clamped `sprite_kind > 2` to slime, so War Machine sprite_kind 3 never displayed.

## Fix
- Restored enemy BG safe tile count to 48 tiles at base 80.
- Added `battle_enemy_bg_load_tiles_for_formation()` to load only the current formation art into tile IDs 80-127.
- L-size golem now uses all 48 safe tiles by itself.
- S/M formations remap source sprite kinds to runtime slots 0-2. War Machine can now be loaded into one of those safe runtime slots.
- Fixed main.c sprite_kind clamp from >2 to >4 before runtime remapping.
