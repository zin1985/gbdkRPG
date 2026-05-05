# SAFE EVENT POLICY 049

## Current confirmed chain

- rpg046: inactive actor slot worked.
- rpg047: visible actor slot worked.
- rpg048: talkable actor + try_interact() branching + new message string white-screened.
- rpg048b: talkable actor data-only worked.

## Interpretation

The likely risky part is not `talkable=1` by itself.

More likely risky items:

- extra logic added inside `try_interact()`
- new Japanese string data
- code/data layout shift
- bank boundary pressure
- link-time placement that compiles but boots badly

## Safe feature additions for now

Prefer:

1. data-only actor changes
2. existing message reuse
3. README/CHECK/comment-only changes
4. existing tile reuse
5. no new VRAM/OAM loading path

Avoid:

1. actor addition + interaction code change in same build
2. new string + branch addition in same build
3. MAP_GFX_TILE_COUNT change
4. new sprite sheet
5. WINDOW/printf/gotoxy/screen debug
6. raw VRAM/LCDC debug writes

## Next safe implementation candidate

rpg050 candidate:

- Keep actors[] unchanged from rpg048b.
- Add one very small message constant only.
- Do not change try_interact().
- Or, alternatively, change try_interact() only while reusing existing string.
- Do not do both in the same build.
