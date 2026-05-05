# BANK FIX 059

## Why

rpg058 still showed `romusage` warnings for non-banked areas.

A likely Bank 0 pressure source is the use of console/stdio screen helpers and formatted output.

## Fix

rpg059 removes console/stdio usage from main.c.

Replaced with tiny local helpers:

- `screen_clear()`
- `put_ascii()`
- `put_cursor()`
- `u8_to_dec()`
- `put_u8()`
- `put_hp_pair()`

Battle damage messages were simplified to fixed text to avoid formatted-output support code.

## Also fixed

`map_data.h` had a macro collision:

```c
#ifndef MAP_DATA_H
#define MAP_DATA_H

#define MAP_DATA_H 16u
```

This caused a redefinition warning.

rpg059 changes this to:

```c
#ifndef MAP_DATA_H_INCLUDE
#define MAP_DATA_H_INCLUDE

#define MAP_DATA_WIDTH  16u
#define MAP_DATA_HEIGHT 16u
```
