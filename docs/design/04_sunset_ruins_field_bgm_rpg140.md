# CHECK_rpg140_sunset_ruins_field_bgm

## Purpose

Use the uploaded `sunset_ruins_field_bgm.c/.h` as the field BGM while keeping the existing heavy metal Celtic battle BGM as the battle track.

## Changed files

- `audio.c`
  - Added `#include "sunset_ruins_field_bgm.h"`.
  - Replaced the old tiny built-in field melody with the banked sunset ruins field BGM driver.
  - `AUDIO_TRACK_FIELD` now calls `sunset_ruins_field_bgm_init()` on start.
  - `audio_update()` now calls `sunset_ruins_field_bgm_update()` every frame while the field track is active.
  - Track switching now stops either the field BGM or the battle BGM before starting another track.
  - The small town BGM remains as the simple built-in fallback track.

- `sunset_ruins_field_bgm.c`
  - Newly added.
  - Placed in ROM bank 14 with `#pragma bank 14` so the large sequence data does not increase Bank 0.

- `sunset_ruins_field_bgm.h`
  - Newly added.
  - Public functions are declared `BANKED`.

- `Makefile`, `build.sh`, `build.bat`
  - Output name changed to `rpg140_sunset_ruins_field_bgm.gb`.
  - Added `sunset_ruins_field_bgm.c` to the source list.

## Not changed

- Battle BGM remains `heavy_metal_celtic_battle_bgm.c` in bank 15.
- Town BGM remains the existing simple internal tune.
- Field map, dungeon map, NPC, battle logic, equipment, quest, and debug no-encounter accessory behavior were not changed.

## Bank policy

- Field BGM data is placed in bank 14.
- Battle BGM data remains in bank 15.
- `audio.c` remains only the dispatcher plus small town fallback table.
- The old field melody table was removed from `audio.c` to avoid growing Bank 0.

## Local build

Run:

```sh
./build.sh
```

Expected ROM:

```text
rpg140_sunset_ruins_field_bgm.gb
```

## ChatGPT environment note

The ChatGPT container does not have `/opt/gbdk/bin/lcc`, so the final ROM build was not executed here. `tools/prebank_check.py` was run and did not report the usual Bank 0 direct graphics/message issues; it still reports informational bank verification notes that require the real compiler/linker and romusage output.
