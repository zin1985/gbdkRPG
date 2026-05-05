# BATTLE ENCOUNTER TABLE 071

rpg071 fixes battle-screen residue and adds a small encounter table with up to two enemies.

## Fixes

- Full 32x32 BG clear before battle screen drawing.
- Battle command UI is boxed using the same JP frame tiles as the message window.
- Enemy names are shown only in the lower-left enemy box.
- Existing enemy OBJ tile sheet is reused as battle enemy sprites.
- Up to two enemies can appear at once.

## Data

Enemy and encounter data live in bank 7:

- `battle_data_bank.c`
- `battle_data_runtime.c`
- `battle_data.h`

## Safety

No new BG tiles, MAP_GFX_TILE_COUNT change, map actors, object kinds, or sprite-sheet load paths were added.
