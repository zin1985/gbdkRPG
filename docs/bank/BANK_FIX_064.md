# BANK FIX 064

## Problem in rpg063

rpg063 worked until opening messages, then froze during NPC dialogue and town transition/message paths.

Likely cause:

- jpfont.c was BANKED in bank 5.
- misakiUTF16.c was BANKED in bank 6.
- Dialogue rendering called banked jpfont functions, which then called multiple banked misaki functions.
- This created a fragile nested banked-call path inside the text renderer.

## Fix

rpg064 reverts jpfont.c to normal non-banked code.

```text
jpfont.c:
  non-banked again

misakiUTF16.c:
  still bank 6
```

jpfont.c now uses only one BANKED boundary into misaki:

```c
getFontData(font8, p, 1);
```

It no longer directly calls:

- charUFT8toUTF16()
- utf16_HantoZen()

The glyph cache key is now byte-based instead of normalized UTF-16 based.  
That is less perfect, but safer and sufficient for rendering.

## Expected result

- Dialogue window should open without freezing.
- NPC dialogue should render.
- Town transition/message paths should no longer freeze.
- Bank 6 keeps the large Misaki font data out of Bank 0.
