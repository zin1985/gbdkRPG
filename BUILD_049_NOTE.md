# BUILD 049 NOTE

Base:
- rpg048b_talkable_data_only, confirmed working.

Purpose:
- Freeze the newly discovered safety rule:
  data-only talkable actor changes are OK, but try_interact() branching/string growth may be risky.

Runtime behavior:
- No intended behavior change from rpg048b.

Changes:
- Comment-only safe event policy added near interaction handling.
- README/CHECK documentation updated.
- Build output renamed to rpg049_safe_event_policy.gb.

What this version intentionally does NOT do:
- No new event logic.
- No new actor.
- No new message flow.
- No new sprite graphics.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No VRAM/OAM load path changes.
- No debug overlay/WINDOW/printf/gotoxy.

Expected result:
- It should behave the same as rpg048b.
- If this white-screens, the issue is likely build/layout sensitivity from source-size/doc/build script changes or the wrong base file was used.
