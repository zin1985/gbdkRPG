# Program Flow Debug Notes

## Startup

```text
main
  BGP/OBP setup
  dialogue_init
  init_game
  load_map_mode
```

## Map load

```text
load_map_mode
  DISPLAY_OFF
  SHOW_BKG
  SHOW_SPRITES
  HIDE_WIN
  snap_camera_to_player
  draw_object_map
  apply_camera_scroll
  init_map_sprites
  DISPLAY_ON
```

## BG draw

```text
draw_object_map
  fill 32x32 BG buffer with floor
  iterate object map 16x16
  convert object kind to 2x2 BG tile IDs
  apply field-only forest overlay
  set_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles)
  set_bkg_tiles(0,0,32,32,bg)
```

## Sprite load

```text
init_map_sprites
  SPRITES_8x8
  set_sprite_data player 0-31
  set_sprite_data npc    32-63
  set_sprite_data enemy  64-95
  draw_all_actors
```

## Safe known behavior

rpg029/rpg030 line keeps:

- MAP_GFX_TILE_COUNT = 17
- no new OAM
- town marker and forest are separate BG tiles
- no soft mountain yet
- no actor mutation during area switch
