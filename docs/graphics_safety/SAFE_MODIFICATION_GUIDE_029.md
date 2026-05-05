# SAFE MODIFICATION GUIDE 029

Use this order for future changes:

## Rule 1: one risk category per version

Do not mix these in one build:

- BG tile count changes
- object/collision map changes
- actor/NPC logic changes
- sprite/OAM additions
- map size changes

## Rule 2: if adding visual-only BG

Safe:
- Reuse existing tile slots, or add exactly four BG tiles.
- Do not add collision.
- Do not add object kind.
- Draw as overlay in `draw_object_map()`.

Unsafe:
- Add BG tiles + new object kind + collision + actor changes together.

## Rule 3: if adding NPC behavior

Safe:
- Reuse existing NPC actor slot.
- Change only the message based on current area.
- Do not mutate actor array during boot.

Unsafe:
- Change actor active/solid/tile/sprite in `init_game()` or map restore while BG debugging.

## Rule 4: if adding field expansion

Safe:
- Change only collision/object rows.
- No tile count changes in same build.

Unsafe:
- Field expansion + new tiles + NPC switching in one build.

## Recommended next builds

- rpg030: verify rpg029 baseline.
- rpg031: add only soft mountain by reusing existing forest or mountain tiles.
- rpg032: add real soft mountain BG tiles only, no actor/map logic changes.
- rpg033: add town NPC text branch only.
- rpg034: expand field walkable area only.
