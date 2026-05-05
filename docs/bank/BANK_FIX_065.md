# BANK FIX 065

## Problem

rpg064 still froze when opening messages.

The remaining risky part was the text renderer calling a BANKED Misaki function while dialogue was being drawn.

## Fix

rpg065 removes BANKED function calls from the dialogue text path.

```text
jpfont.c:
  non-banked

misakiUTF16.c:
  non-banked code

font_data_bank.c:
  bank 6, contains the huge Misaki font tables

font_data_runtime.c:
  NONBANKED wrappers that manually switch to bank 6, copy a few bytes, restore bank
```

The dialogue renderer now calls normal functions only. Only tiny data-read wrappers switch banks.
