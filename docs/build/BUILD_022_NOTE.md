rpg022
- Base: rpg017
- Goal: draw one passable forest marker as BG, not sprite.
- Implementation: draw_object_map() overlays one 16x16 marker onto bg[] before set_bkg_tiles().
- Uses existing MAP_TILE_TOWN_* tiles as temporary forest tiles.
- No MAP_GFX_TILE_COUNT change.
- No new object kind.
- No collision change.
- No sprite/OAM changes.
