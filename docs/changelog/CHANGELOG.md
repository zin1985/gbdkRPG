# CHANGELOG

## rpg017

- Based on rpg015 safe rollback.
- Removed position/camera debug rows from status page to reduce main.c size.
- Added `-Wl-m -Wl-j` to build.bat/build.sh/Makefile for map/symbol output.
- No sprite additions in this version.
- No BG tile/object kind/actor/VRAM path changes.


## rpg022
- Based on rpg017 no-sprite safety line.
- Added a visual-only BG forest marker overlay at one field coordinate.
- Reuses existing BG tiles; no new BG tiles and no MAP_GFX_TILE_COUNT change.
- No collision change, no new object kind, no sprite/OAM changes.


## rpg024
- Restored/kept the town entrance marker BG tiles at slots 9-12.
- Added new forest BG tiles at slots 13-16 so town and forest are visually distinct.
- Forest remains a visual-only overlay: no collision and no new object kind.
- MAP_GFX_TILE_COUNT changed from 13u to 17u. This is the smallest necessary BG tile increase to keep both town and forest graphics.


## rpg029
- Rebased to rpg024-style last stable visual baseline.
- Added detailed safe-modification comments to main.c.
- Added PROGRAM_FLOW_DEBUG_029.md and SAFE_MODIFICATION_GUIDE_029.md.
- Removed rpg025-rpg028 risky changes by using rpg024 as the base.
- No runtime feature change intended from rpg024.


## rpg030
- Graphics architecture stabilization build.
- No new terrain, NPC, battle enemy, or OAM feature added.
- Added fixed safety checkpoints to main.c and sprites.h.
- Added GRAPHICS_ARCHITECTURE_030.md, SAFE_GRAPHICS_CHECKPOINT_030.md, and PROGRAM_FLOW_DEBUG_030.md.
- Updated build outputs to rpg030.gb.
- Keeps the rpg029 known-good graphics behavior.


## rpg046
- Reverted to pure rpg030 base after rpg045_fix white-screened.
- Tests future OAM 12-15 through data-only inactive Actor entry.
- No new functions were added.
- No direct new move_sprite() calls were added.
- No set_sprite_tile()/set_sprite_data() changes.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- Actor count becomes 3, but the new actor is active=0 and invisible.


## rpg047
- Based on confirmed-working rpg046.
- Changed the added actor slot from inactive to visible.
- Uses existing NPC0 graphics.
- Uses OAM 12-15 via the existing actor pipeline.
- The new actor is non-solid and non-talkable.
- No new sprite graphics.
- No new set_sprite_data().
- No new set_sprite_tile() calls outside existing actor flow.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.


## rpg048b
- Based on confirmed-working rpg047.
- Data-only talkable test for the added OAM 12-15 actor.
- Changed the third actor talkable flag from 0u to 1u.
- Did not change try_interact().
- Reused a shared existing NPC message to avoid adding a new long string.
- No new sprite graphics.
- No set_sprite_data() changes.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No collision change.


## rpg049
- Based on confirmed-working rpg048b.
- No gameplay behavior change.
- Added safe event policy comments and documentation.
- Added guidance that rpg048 white-screen was likely caused by interaction code/string/layout growth, not talkable=1 alone.
- No new actor/NPC.
- No new sprite graphics.
- No set_sprite_data() changes.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No object kind or collision change.


## rpg051
- Based on confirmed-working rpg049.
- No game behavior change.
- Reworked build scripts to output build.log.
- Added warning/overflow keyword checks to build scripts.
- Added optional romusage checks when romusage is available.
- Output ROM name changed to rpg051_build_romusage_check.gb.
- No main.c behavior change.
- No new actor/NPC.
- No new message strings.
- No sprite/BG/collision changes.


## rpg055
- Based on rpg051/rpg049 stable line.
- Applies rpg052-rpg055 message banking plan in one build.
- Added messages.h.
- Added messages_bank.c in ROM bank 2.
- Added messages_runtime.c with NONBANKED wrapper and WRAM copy buffer.
- Actor now stores message_id instead of const char *message.
- Existing common NPC message moved out of main.c into messages_bank.c.
- Short test message is added to messages_bank.c, not main.c.
- try_interact() now calls message_show(actor.message_id).
- Build scripts now treat dangerous bank-span/overflow warnings as fatal.
- Added tools/prebank_check.py as a pre-compile heuristic check.


## rpg056
- Based on rpg055.
- rpg055 still overflowed from bank 1 into bank 2.
- Moved graphics asset arrays in sprites.c to ROM bank 3.
- Added banked_graphics.h/c.
- Added NONBANKED wrappers for set_bkg_data() and set_sprite_data().
- main.c now loads graphics through wrappers using BANK(sprite_data_bank).
- Build scripts include banked_graphics.c.
- This aims to free bank 1 by moving the largest asset file out of the overflowing bank.


## rpg057
- Based on rpg056.
- Fixed false-positive build failure in dangerous linker warning check.
- build scripts now keep precheck_output.log, compile_output.log, and build.log separate.
- dangerous warning grep only scans compile_output.log, not build.log.
- Avoids the previous `grep: build.log: input file is also the output` problem.
- prebank_check.py now strips C comments before checking direct set_bkg_data()/set_sprite_data().
- prebank_check.py avoids printing fatal linker phrases verbatim.
- No gameplay/source behavior change.


## rpg058
- Continued bank cleanup after rpg057 romusage warnings.
- Moved collision/object map arrays out of main.c into map_data_bank.c.
- Added map_data.h.
- Added map_data_runtime.c NONBANKED wrappers.
- main.c now reads map data through map_data_collision16_at() and map_data_object16_at().
- Build scripts now treat dangerous romusage warnings as fatal, not only compiler output warnings.
- No new gameplay feature, sprite sheet, BG tile, or object kind.


## rpg059
- Continued Bank 0 cleanup after rpg058 romusage warnings.
- Removed console/stdio/string includes from main.c.
- Replaced console screen rendering with tiny jpfont-based helpers.
- Replaced formatted battle messages with fixed dialogue strings to avoid stdio pull-in.
- Added u8_to_dec(), put_u8(), put_hp_pair(), put_cursor(), put_ascii(), screen_clear().
- Fixed map_data.h include guard conflict by renaming the guard to MAP_DATA_H_INCLUDE and dimensions to MAP_DATA_WIDTH/MAP_DATA_HEIGHT.
- No new BG tiles, actors, object kinds, or gameplay content.


## rpg060
- Based on rpg059.
- Fixes link errors from missing helper function definitions.
- Added bodies for:
  - put_ascii()
  - put_cursor()
  - u8_to_dec()
  - put_u8()
  - put_hp_pair()
  - screen_clear()
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No actor/NPC addition.
- No object kind addition.
- No VRAM load path change.


## rpg061
- Continued Bank 0 cleanup after rpg060 romusage warnings.
- Moved jpfont.c to ROM bank 5.
- Moved misakiUTF16.c to ROM bank 5 with jpfont.c.
- Added BANKREF(jpfont_bank) and BANKREF(misaki_bank).
- Marked public jpfont APIs as BANKED in jpfont.h and jpfont.c.
- Kept misakiUTF16 functions unbanked-in-bank because they are called internally by jpfont while bank 5 is active.
- No BG tile additions.
- No MAP_GFX_TILE_COUNT change.
- No OAM/actor/object kind additions.


## rpg062
- Based on rpg061.
- Fixes misakiUTF16.c compile error caused by using BANKREF() without including gb/gb.h.
- Added `#include <gb/gb.h>` to misakiUTF16.c before BANKREF(misaki_bank).
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No OAM/actor/object kind changes.
- jpfont/misaki bank 5 strategy remains unchanged.


## rpg063
- Based on rpg062.
- Fixes bank 5 -> bank 6 overflow caused by placing jpfont.c and misakiUTF16.c in the same bank.
- jpfont.c remains in ROM bank 5.
- misakiUTF16.c is moved to ROM bank 6.
- misakiUTF16 public functions are now marked BANKED in both header and source.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No OAM/actor/object kind changes.


## rpg064
- Based on rpg063.
- Fixes runtime freeze when opening dialogue / town message paths.
- Reverted jpfont.c and jpfont.h public APIs back to non-banked.
- Kept misakiUTF16.c in ROM bank 6 to keep large font data out of Bank 0.
- Changed jpfont.c to call only getFontData() as the BANKED boundary into misaki.
- Removed direct jpfont calls to charUFT8toUTF16() / utf16_HantoZen().
- Only getFontData() remains BANKED in misakiUTF16.h/c.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No OAM/actor/object kind changes.


## rpg065
- Based on rpg064.
- Fixes continued freeze when opening NPC/town messages.
- Removed BANKED runtime calls from the text renderer path.
- jpfont.c remains non-banked.
- misakiUTF16.c code is non-banked again.
- Large Misaki font tables are moved to font_data_bank.c in ROM bank 6.
- Added font_data_bank.h/c and font_data_runtime.c.
- misakiUTF16.c accesses font tables only through NONBANKED wrappers.
- No BG tile/MAP_GFX_TILE_COUNT/OAM/actor/object kind changes.


## rpg066
- Based on rpg065.
- Fixes build error in misakiUTF16.c where getFontTableAddress() still returned raw `fdata`.
- Since fdata now lives in font_data_bank.c bank 6, getFontTableAddress() now returns NULL for compatibility.
- Updated prebank_check.py to match the rpg065/rpg066 font data split design.
- No BG tile changes.
- No MAP_GFX_TILE_COUNT change.
- No OAM/actor/object kind changes.


## rpg067
- Based on rpg066_fontdata_gettable_fix, the confirmed message-stable version.
- Added area-based actor visibility using existing actors only.
- AREA_FIELD now draws/blocks field-visible enemy actors and hides NPC actors.
- AREA_TOWN now draws/talks to NPC actors and hides enemy actors.
- actor_at_tile() ignores actors hidden in the current area.
- find_talkable_actor() ignores actors hidden in the current area.
- draw_all_actors() hides OAM for actors not visible in the current area.
- No new actors.
- No new NPCs.
- No new BG tiles.
- No MAP_GFX_TILE_COUNT change.
- No object kind change.
- No sprite sheet / VRAM load path change.


## rpg068
- Based on rpg067_area_actor_visibility.
- Added field random encounters.
- Existing ENEMY actor data is used as the random battle template.
- Field enemy actor is hidden from the map; encounters occur after completed field steps.
- Added encounter_grace_steps to avoid immediate repeat encounters after boot/town/battle.
- Added palette-flash battle start effect.
- No new BG tiles.
- No MAP_GFX_TILE_COUNT change.
- No new actors/NPCs.
- No new object kind.
- No sprite sheet or VRAM load path changes.


## rpg069
- Based on rpg068_field_random_encounter_effect.
- Fixes battle/dialogue text corruption caused by the temporary byte-based jpfont cache key.
- Restored normalized UTF-16 glyph cache key in jpfont.c.
- Battle screen now resets jpfont cache while LCD is off before drawing battle text.
- Battle opening message now redraws battle frame/menu after the dialogue closes.
- Removed decorative ASCII border rows from battle frame to reduce glyph cache pressure.
- Restores jpfont cache/window after returning to map while LCD is off.
- No new BG tiles.
- No MAP_GFX_TILE_COUNT change.
- No new actors/NPCs.
- No object kind changes.
