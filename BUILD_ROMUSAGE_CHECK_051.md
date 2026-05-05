# BUILD ROMUSAGE CHECK 051

## Purpose

rpg050 white-screened after adding only a very short message string.

This suggests that the problem may be related to:

- ROM bank boundary pressure
- fixed bank / HOME area pressure
- linker layout changes
- literal/string area growth
- ASlink warnings that compile successfully but produce unsafe ROM layout
- undetected overflow into an empty bank

rpg051 does not change gameplay.  
It only strengthens the build system to make these issues visible.

## What changed

- build.sh writes `build.log`
- Makefile writes `build.log`
- build.bat writes `build.log`
- all builds still use:
  - `-Wl-m`
  - `-Wl-j`
  - `-Wl-yt0x19`
  - `-Wl-yo8`
- build scripts search the log for:
  - warning
  - error
  - overflow
  - ASlink
  - bank
  - relocation
  - segment
  - area
  - spans
- if `romusage` is available, build scripts run:
  - `romusage <map> -a -g -B`
  - `romusage <map> -q -E`

## Important warning

`Warning: Write from one bank spans into the next`

This warning is especially important.  
If this appears, treat the ROM as unsafe even if `.gb` is generated.

## Recommended comparison

Build rpg049 and rpg051/rpg050 candidates and compare:

```sh
romusage rpg049_safe_event_policy.map -a -g -B > usage049.txt
romusage rpg051_build_romusage_check.map -a -g -B > usage051.txt
diff -u usage049.txt usage051.txt
```

For the white-screening rpg050, compare the areas around `_CODE`, `_HOME`, `_LIT`, `_INITIALIZER`, and bank end addresses.

## This version intentionally does not change

- main.c behavior
- actor count
- message strings
- try_interact()
- sprite data
- BG data
- MAP_GFX_TILE_COUNT
- collision/object maps
