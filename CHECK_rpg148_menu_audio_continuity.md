# rpg148_menu_audio_continuity

## Purpose
Improve BGM continuity during menu/status/item/equipment screen redraws.

## Cause addressed
The BGM players advance from audio_update(). Many JP font/window drawing helpers perform large set_bkg_tiles/set_win_tiles loops without reaching audio_wait_vbl(), so music can feel interrupted while opening menus or redrawing pages.

## Changes
- jpfont.c now includes audio.h.
- Added jp_audio_busy_tick(), which services audio every 64 tile/font writes during busy UI drawing.
- Large JP UI helpers now call jp_audio_busy_tick() while drawing:
  - jp_window_prepare
  - jp_window_clear_text
  - jp_bkg_clear_area
  - jp_draw_bkg_frame
  - jp_put_bkg_text
  - jp_put_glyph_utf8
  - jp_cache_store
- jp_wait_vbl() now uses audio_wait_vbl() instead of raw wait_vbl_done().

## Notes
This does not change BGM assignment or scene logic. It only keeps the active BGM serviced during blocking UI redraws.

## Output
rpg148_menu_audio_continuity.gb
