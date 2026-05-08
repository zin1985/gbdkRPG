# rpg134_bank0_final_14b_trim

## Purpose
rpg133 compiled, but romusage still reported a very small Bank 0 overflow around
_INITIALIZER / _GSINIT / _GSFINAL. This version trims additional unused Bank 0
helper code without adding gameplay data or new features.

## Changes
- Output ROM name changed to `rpg134_bank0_final_14b_trim.gb`.
- Removed unused `u8_to_dec()` helper from `main.c`.
- Removed unused `put_u8()` helper from `main.c`.
- Removed unused `battle_put_u8()` wrapper.
- Removed `battle_put_u16()` wrapper and replaced its two call sites with direct `put_u16(BATTLE_BG_X(...))` calls.

## Expected effect
This should free more than the remaining ~14 bytes that caused rpg133's final
romusage warning.

## Not changed
- No map data changed.
- No message text changed.
- No sprite/BG data changed.
- No gameplay feature removed.
- Enemy BG rendering remains unchanged.
