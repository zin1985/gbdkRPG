# FONT CACHE FIX 069

## Problem observed in rpg068

The game ran, but battle and dialogue text could corrupt:

- battle screen text became wrong glyphs
- battle BG looked noisy
- dialogue text could display wrong characters
- map return sometimes retained battle-looking text

## Main cause

rpg064-rpg068 used a temporary byte-based jpfont cache key to avoid nested BANKED calls.

That prevented freezing, but different UTF-8 characters can collide or reuse the wrong tile.
Once the cache reuses a tile, already placed BG/window characters can appear as the wrong glyph.

## Fix

rpg069 restores the normalized UTF-16 cache key:

```text
UTF-8
  -> charUFT8toUTF16()
  -> utf16_HantoZen()
  -> cache_key
```

This is safe again because misakiUTF16.c code is non-banked in the rpg066+ line.
Only raw font table bytes are read through the banked data wrapper.

## Additional cleanup

- reset jpfont cache before drawing battle screen
- redraw battle frame/menu after opening dialogue closes
- remove decorative ASCII border rows from battle screen
- reset jpfont cache/window when restoring map mode

## Not changed

- no BG tile increase
- no sprite/OAM change
- no actor addition
- no object kind addition
