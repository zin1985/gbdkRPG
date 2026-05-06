# CHECK rpg090_battle_window_fix

## Purpose
Fix battle window/UI offset by restoring the battle BG origin to the true visible origin.

## Must check in VBA / emulator
- [ ] Battle entry does not white-screen.
- [ ] Top party/status window begins at the left edge and top row.
- [ ] Enemy-name window begins at the left side around row 11.
- [ ] Command window is aligned with the enemy-name window and not shifted sideways.
- [ ] Message strip appears at the bottom rows and is not horizontally offset.
- [ ] Cursor OBJ aligns with the command text.
- [ ] Field map returns normally after battle.
- [ ] No field-camera scroll residue appears in battle.

## Not changed
- MAP_GFX_TILE_COUNT
- jpfont.c
- misakiUTF16.c
- BGM
- OAM allocation
- New BG tiles / actor / object kind
