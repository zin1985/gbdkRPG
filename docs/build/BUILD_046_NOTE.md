# BUILD 046 NOTE

Reason for this approach:
- rpg045_fix white-screened even though it only added a small hide_test_sprite() function.
- That suggests code-size/layout sensitivity may be the real issue, not OAM 12-15 itself.

Change:
- Do not add any new runtime function.
- Add one inactive actor entry to the existing `actors[]` table.
- The existing `draw_all_actors()` loop already hides inactive actors.
- The new entry uses:
  - active = 0
  - sprite_base = 12
  - tile_base = NPC0_TILE_BASE

What this tests:
- Whether actor/OAM slot expansion can be handled through the existing path without new code.

What this intentionally does not do:
- No new sprite graphics.
- No new set_sprite_data().
- No new set_sprite_tile().
- No new direct move_sprite() calls.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No object kind or collision changes.

Expected result:
- The game should look the same as rpg030.
- No new sprite should appear.
- If this white-screens, even actor table growth is unsafe at this point.
