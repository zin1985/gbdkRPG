# BANK FIX 058

## Why

rpg057 removed direct compiler/linker bank-span warnings, but `romusage` still warned that non-banked areas may overflow beyond Bank 0.

## What moved

The following arrays were moved out of main.c:

- collision16_map
- object16_map
- town_collision16_map
- town_object16_map

They now live in `map_data_bank.c` under ROM bank 4.

## Runtime access

main.c calls:

```c
map_data_collision16_at(current_area, tx, ty);
map_data_object16_at(current_area, tx, ty);
```

These are NONBANKED wrappers in `map_data_runtime.c`.

## Build gate

rpg058 treats romusage warnings as fatal:

- Possible overflow beyond Bank 0
- Possible overflow
- Multiple write
- Write from one bank spans into the next

If any appear, the build exits with error before the ROM is treated as safe.
