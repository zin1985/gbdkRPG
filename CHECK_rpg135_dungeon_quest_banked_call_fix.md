# rpg135_dungeon_quest_banked_call_fix

## Purpose
Fix runtime crash when entering dungeon / ruins after world feature expansion.

## Root cause
`quest.c` is compiled into ROM bank 7 using `#pragma bank 7`, but public APIs in `quest.h` were declared without `BANKED`.
Calls such as `quest_start(QUEST_LOST_KEY)` from `main.c` could be emitted as normal near calls instead of far/banked calls, causing a jump into the wrong bank at runtime.

## Changes
- Marked all public quest APIs in `quest.h` as `BANKED`.
- Marked corresponding function definitions in `quest.c` as `BANKED`.
- Changed output ROM name to `rpg135_dungeon_quest_banked_call_fix.gb`.

## Not changed
- No map data changes.
- No BG tile changes.
- No enemy BG rendering changes.
- No dungeon/ruins event coordinates changed.
- No new features added.

## Verification target
1. Build should still pass compile/link.
2. Bank 0 overflow should remain at least no worse than rpg134.
3. Entering dungeon should no longer crash at `quest_start(QUEST_LOST_KEY)`.
4. Entering ruins should no longer crash at `quest_advance(QUEST_LOST_KEY)`.
