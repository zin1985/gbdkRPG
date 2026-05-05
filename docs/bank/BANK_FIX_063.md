# BANK FIX 063

## Problem in rpg062

rpg062 built far enough to show a real bank overflow:

```text
Warning: Write from one bank spans into the next. 0x17fe4 -> 0x18003 (bank 5 -> 6)
```

That means putting both `jpfont.c` and `misakiUTF16.c` in bank 5 exceeded the 16KB bank boundary by a small amount.

## Fix

Separate the Japanese text stack into two banks:

```text
bank 5:
  jpfont.c

bank 6:
  misakiUTF16.c
  misakiUTF16FontData.h data included by misakiUTF16.c
```

Because jpfont.c in bank 5 calls functions in misakiUTF16.c bank 6, the public misaki functions are now marked `BANKED`.

## Changed prototypes

The following are BANKED now:

- findcode()
- getFontDataByUTF16()
- hkana2kana()
- utf16_HantoZen()
- charUFT8toUTF16()
- Utf8ToUtf16()
- getFontData()
- getFontTableAddress()

## Expected result

The previous bank 5 -> 6 span warning should disappear.  
After that, still check romusage for Bank 0 warnings.
