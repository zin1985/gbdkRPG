# BANK FIX 061

## Why

rpg060 removed console/stdio usage but romusage still reported possible non-banked overflow beyond Bank 0.

The next major code/data pressure point is the Japanese font rendering stack:

- jpfont.c
- misakiUTF16.c
- misakiUTF16FontData.h data included by misakiUTF16.c

## Fix

rpg061 moves jpfont.c and misakiUTF16.c into ROM bank 5.

```c
#pragma bank 5
BANKREF(jpfont_bank)
BANKREF(misaki_bank)
```

The public jpfont APIs are marked BANKED:

```c
void jp_put_bkg_text(...) BANKED;
void jp_window_prepare(void) BANKED;
...
```

Gameplay code should continue calling jpfont APIs normally.  
GBDK's banked call support handles the switch.

## Why misaki functions are not marked BANKED

misakiUTF16.c is placed in the same bank as jpfont.c.

Only jpfont.c should call misakiUTF16 functions.  
Because jpfont is already executing in bank 5, internal calls to misaki functions in the same bank can stay normal.

## Risk

This is a larger banking change than data-only moves.

Confirm carefully:

- build succeeds
- romusage warnings are gone or reduced
- boot does not white-screen
- dialogue text appears
- menu/status/battle text appears
