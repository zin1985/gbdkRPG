# BANK FIX 056

## Problem observed in rpg055

```text
Warning: Write from one bank spans into the next. 0x7fe8 -> 0x8003 (bank 1 -> 2)
Warning: Multiple write ...
Warning: Possible overflow from Bank 1 into Bank 2
```

Moving only message text was not enough. Bank 1 still overflowed into bank 2.

## Fix

`sprites.c` contains the largest ROM asset arrays, so rpg056 moves it to ROM bank 3.

```c
#pragma bank 3
BANKREF(sprite_data_bank)
```

Because banked data must be active while being read, main.c no longer calls `set_bkg_data()` / `set_sprite_data()` directly for graphics assets.

Instead it calls NONBANKED wrappers:

```c
set_banked_bkg_data(..., BANK(sprite_data_bank));
set_banked_sprite_data(..., BANK(sprite_data_bank));
```

These wrappers save `CURRENT_BANK`, switch to the graphics bank, copy to VRAM, and restore the previous bank.

## About pre-compile bank overflow checks

Exact overflow cannot be known before compile/link because the linker decides final layout.

The precheck can only warn about risky source patterns.  
Final acceptance requires build.log + romusage.
