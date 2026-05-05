# BUILD 060 NOTE

## Problem in rpg059

The build failed with undefined globals:

```text
_put_u8
_screen_clear
_put_ascii
_put_cursor
_put_hp_pair
```

This means main.c had prototypes and call sites, but the helper function bodies were missing.

## Fix

rpg060 adds the missing static helper definitions to main.c.

## Not changed

- No BG tile changes
- No MAP_GFX_TILE_COUNT change
- No sprite/OAM changes
- No actor/NPC changes
- No object kind changes
- No new gameplay content

## Check

Build should pass the previous undefined-global stage.  
After that, judge by:

- compile_output.log
- romusage_output.log
- VBA boot test
