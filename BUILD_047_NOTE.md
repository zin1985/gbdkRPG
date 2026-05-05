# BUILD 047 NOTE

Base:
- rpg046_inactive_actor_slot, confirmed working.

Purpose:
- Display the reserved OAM 12-15 slot using the existing actor pipeline.

Change:
- The third actor entry changes:
  - active: 0 -> 1
  - solid: 0
  - talkable: 0
  - sprite_base: TEST_ACTOR_SPRITE_BASE = 12
  - tile_base: NPC0_TILE_BASE

What this version intentionally does NOT do:
- No new sprite graphics.
- No new sprite sheet.
- No new set_sprite_data().
- No BG tile change.
- No MAP_GFX_TILE_COUNT change.
- No collision change.
- No dialogue/message added for this actor.

Expected behavior:
- A second NPC-looking sprite appears at field tile (10,8).
- It should not block movement.
- It should not start conversation.
- Existing player, NPC, enemy, town, and battle should still work.
