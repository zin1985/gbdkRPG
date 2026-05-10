# rpg190 cursor unified

- Unified cursor graphics to the user-provided 8x8 cursor.
- Replaced the `>` glyph (U+003E) in `misakiUTF16FontData.h` so BG/text cursors use the new shape.
- Replaced `battle_cursor_tiles` in `sprites.c` so the battle OBJ cursor matches the same shape.
- No functional logic changes.
- `main.c` unchanged.
