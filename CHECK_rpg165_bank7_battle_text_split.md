# rpg165_bank7_battle_text_split

## Purpose
Fix the remaining Bank 7 overflow after rpg164.

## Changes
- Moved `battle_text.c` from `#pragma bank 7` to `#pragma bank 10`.
- Kept Bank 0 impact minimal because the public battle text functions are already declared `BANKED`.
- Updated output name to `rpg165_bank7_battle_text_split.gb`.

## Expected effect
Bank 7 had only a few bytes of overflow (`0x1ffe3 -> 0x20002`). Moving `battle_text.c` should free much more than the overflow amount and avoid the bank 7 -> 8 span warning.
