# rpg030 Graphics Architecture

## Goal

This build does not add new gameplay features. It rebuilds the graphics process
around fixed safety rules so future expansion does not drift into white-screen
failures.

## Boot flow

```text
main()
  -> dialogue_init()
  -> init_game()
  -> load_map_mode()
       -> DISPLAY_OFF
       -> snap_camera_to_player()
       -> draw_object_map()
            -> build 32x32 BG buffer
            -> set_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles)
            -> set_bkg_tiles(...)
       -> init_map_sprites()
            -> set_sprite_data() for actor OBJ sheets
            -> draw_all_actors()
       -> DISPLAY_ON
```

If the screen is white at boot, debug from `load_map_mode()` downward first.

## BG vs OBJ

### BG/metatile should be used for

- field terrain
- forests
- passable soft mountains
- town markers
- square walls
- pots that do not move
- chests before opening animation
- signs
- save points / recovery springs

### OBJ/OAM sprites should be used for

- player
- NPCs
- walking enemies
- battle enemies
- cursors
- projectiles/effects
- animated chest lid only if needed

## Tile layout

```text
OBJ tile VRAM:
0-31   player
32-63  npc0
64-95  enemy0

BG tile VRAM:
96 + 0     floor
96 + 1-4   square wall
96 + 5-8   sharp mountain
96 + 9-12  town marker
96 + 13-16 forest canopy
```

## Future growth policy

Do not combine these in one build:

- `MAP_GFX_TILE_COUNT` increase
- new object kind
- collision map change
- actor/NPC count change
- OAM slot addition
- map switching logic change

Use one build per risk.

Recommended sequence:

```text
rpg031: increase BG tile count only, no placement
rpg032: place the new BG tile as overlay only
rpg033: add collision/object data only
rpg034: add area-specific NPC message only
rpg035: add one OAM actor slot only
```

## Field expansion

Town remains 16x16. Field should grow later by introducing:

- `FIELD_MAP16_W/H`
- renderer loops based on active area dimensions
- chunk/sector renderer
- BG buffer windowing into the active field
- collision lookup by current area and active map size

The current renderer still uses the stable 16x16 logical area.
