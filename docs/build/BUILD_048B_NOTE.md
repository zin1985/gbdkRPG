# BUILD 048B NOTE

Reason:
- rpg048 white-screened.
- rpg048 changed both:
  1. actor data
  2. try_interact() logic
  3. added a new message string

This version tests only the data-side part.

Base:
- rpg047_visible_actor_slot, confirmed working.

Change:
- Third actor:
  - active = 1
  - solid = 0
  - talkable = 1
  - sprite_base = TEST_ACTOR_SPRITE_BASE = 12
  - tile_base = NPC0_TILE_BASE
- It reuses `msg_common_npc`.
- `try_interact()` is not modified.

Expected:
- The game should boot like rpg047.
- The added NPC should be talkable.
- Because try_interact() is unchanged, talking to this actor may still enter the existing revive-guide flow.
- That is acceptable for this isolation test.

If this works:
- rpg048 failure was likely caused by try_interact() code expansion or new string/data layout, not talkable=1 itself.
