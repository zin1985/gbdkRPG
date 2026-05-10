# rpg164_bank7_overflow_quest_split

## Purpose
Fix rpg163 build failure caused by a dangerous linker warning:

```text
Warning: Write from one bank spans into the next. 0x1fff9 -> 0x20018 (bank 7 -> 8)
```

The ROM itself was generated, but the safety check correctly failed because bank 7 overflowed.

## Change
- Moved `quest.c` from ROM bank 7 to ROM bank 10.
- Kept public quest APIs `BANKED`, so call sites do not need to grow Bank 0.
- Updated output name to `rpg164_bank7_overflow_quest_split.gb`.

## Why this is safe
- `quest.c` is small but enough to remove the few-byte bank 7 overflow.
- Bank 10 was reserved for future systems and should have enough free space.
- This avoids adding new Bank 0 code.

## Not changed
- Save SRAM implementation is unchanged.
- Title `はじめから / つづきから` and save slot selection are unchanged.
- Shop, sea, chest, pot, NPC collision, battle UI, and current BGM setup are unchanged.
