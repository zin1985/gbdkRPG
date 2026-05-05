# rpg015 rollback/fix note

rpg014 still white-screened. Comparison against rpg012/rpg010 showed the only gameplay/source change in rpg014 was main.c decoration sprite code:

- DECO_TEST_SPRITE / DECO_TEST_TILE definitions
- init_deco_sprite_test()
- draw_deco_sprite_test()
- draw_all_actors() calling draw_deco_sprite_test()
- init_map_sprites() calling init_deco_sprite_test()

rpg015 removes all rpg014 decoration sprite additions and returns main.c/sprites.c/sprites.h to the confirmed rpg010 safe sharp mountain state.

Reason judged most likely: rpg014 increased main.c by about 1KB and added a new per-frame sprite path. Because BG tiles, MAP_GFX_TILE_COUNT, object maps, collision maps, build options, and VRAM BG load path were otherwise unchanged, the white screen is isolated to the added decoration sprite code path / code-size bank-boundary risk, not to the sharp mountain BG data.

Safety rules kept:

- MAP_GFX_TILE_COUNT remains 13u
- No new BG tiles
- No new object kind
- No new actor/NPC
- No decoration sprite path
- No BG tile reload path change
- Build flags stay: -msm83:gb -Wl-yt0x19 -Wl-yo8
