# BUILD 024 NOTE

Base: rpg022 safety line.

Goal:
- Keep the field town marker graphics.
- Add a separate dark forest BG metatile.
- Keep forest passable and avoid object/collision changes.

Changes:
- sprites.h: MAP_GFX_TILE_COUNT 13u -> 17u.
- sprites.c: preserve town marker tiles 9-12 and append forest tiles 13-16.
- main.c: MAP_TILE_FOREST_* now points to MAP_TILE_BASE + 13..16.
- draw_object_map() overlay path remains unchanged except it now uses real forest tiles.

Safety:
- No sprite/OAM changes.
- No set_sprite_data changes.
- No new actor/NPC.
- No new object kind.
- No collision16_map changes.
- No map switching changes.
