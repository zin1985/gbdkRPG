#pragma bank 25

#include <gb/gb.h>
#include "jpfont.h"

UINT8 jp_bkg_backbuffer[JP_BKG_BUFFER_W * JP_BKG_BUFFER_H];
UINT8 jp_bkg_buffer_active;

void jp_bkg_buffer_begin(void) BANKED {
    jp_bkg_buffer_active = 1u;
}

UINT8 jp_bkg_buffer_is_active(void) BANKED {
    return jp_bkg_buffer_active;
}

void jp_bkg_buffer_put_tile(UINT8 x, UINT8 y, UINT8 tile) BANKED {
    if (x >= JP_BKG_BUFFER_W || y >= JP_BKG_BUFFER_H) return;
    jp_bkg_backbuffer[((UINT16)y * JP_BKG_BUFFER_W) + x] = tile;
}

void jp_bkg_buffer_commit(void) BANKED {
    if (jp_bkg_buffer_active == 0u) return;
    jp_bkg_buffer_active = 0u;
    DISPLAY_OFF;
    set_bkg_tiles(0u, 0u, JP_BKG_BUFFER_W, JP_BKG_BUFFER_H, jp_bkg_backbuffer);
    DISPLAY_ON;
}
