# CHECK rpg151_bank0_menu_runtime_trim

## Purpose

rpg149 built, but romusage reported Bank 0 overflow around `_HOME` / `_INITIALIZER` / `_GSINIT`.
This version trims Bank 0 by moving the main menu loop and menu drawing helpers out of `main.c` into a banked runtime module.

## Changes

- Added `menu_runtime.c/.h` in bank 5.
- Moved main menu drawing, cursor movement, objective page, and menu input loop to `menu_runtime_open() BANKED`.
- Kept only a thin `open_main_menu()` wrapper in `main.c` for display setup and field VRAM restoration.
- Removed unused `put_ascii()`, `wait_a_pressed()`, and `current_area_is_dungeon_like()` from `main.c`.
- Kept rpg150 Golem L-size and War Machine M-size changes.

## Expected effect

This should free more than the roughly 381 bytes reported as overflowing Bank 0 in rpg149.
