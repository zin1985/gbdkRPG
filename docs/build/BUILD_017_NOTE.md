# BUILD_017_NOTE

rpg017 is an investigation build before re-trying sprite additions.

Changes:

1. main.c lightening
   - Removed status page debug rows for player tile position and camera position.

2. Linker map output
   - Added `-Wl-m -Wl-j` to Windows/Linux/Makefile build commands.

No gameplay/rendering/VRAM/tile/object kind changes are included.

Next step after runtime confirmation: rpg018 adds only these two lines inside init_map_sprites():

```c
set_sprite_tile(12u, NPC0_TILE_BASE);
move_sprite(12u, 80u, 80u);
```
