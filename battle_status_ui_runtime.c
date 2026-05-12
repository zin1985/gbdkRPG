#pragma bank 23

#include <gb/gb.h>
#include "bank_ids.h"
#include "battle_status_ui_runtime.h"
#include "jpfont.h"
#include "party_runtime.h"

BANKREF(battle_status_ui_runtime)

#define BG_DRAW_W 32u
#define BG_DRAW_H 32u
#define PLAYER_HP_MP_MAX 999u

static void status_u16_to_dec3(UINT16 value, char *out) BANKED {
    UINT16 hundreds;
    UINT8 tens;
    UINT8 ones;
    UINT8 i = 0u;

    if (value > PLAYER_HP_MP_MAX) value = PLAYER_HP_MP_MAX;

    hundreds = value / 100u;
    tens = (UINT8)((value / 10u) % 10u);
    ones = (UINT8)(value % 10u);

    if (hundreds != 0u) {
        out[i++] = (char)('0' + (UINT8)hundreds);
        out[i++] = (char)('0' + tens);
    } else if (tens != 0u) {
        out[i++] = (char)('0' + tens);
    }
    out[i++] = (char)('0' + ones);
    out[i] = '\0';
}

static void status_put_u16(UINT8 col, UINT8 row, UINT16 value) BANKED {
    char buf[5];
    status_u16_to_dec3(value, buf);
    jp_put_bkg_text(col, row, buf);
}

static void status_draw_member(UINT8 slot, UINT8 x) BANKED {
    party_put_active_name_battle(slot, x, 1u);
    jp_put_bkg_text(x, 2u, "H ");
    status_put_u16((UINT8)(x + 2u), 2u, party_get_active_hp(slot));
    jp_put_bkg_text(x, 3u, "M ");
    status_put_u16((UINT8)(x + 2u), 3u, party_get_active_mp(slot));
}

void battle_status_ui_runtime_clear_full(void) BANKED {
    static UINT8 bg[BG_DRAW_W * BG_DRAW_H];
    UINT16 i;
    UINT8 blank;

    blank = (UINT8)(JP_FRAME_BASE + 0u);
    for (i = 0u; i < (UINT16)(BG_DRAW_W * BG_DRAW_H); i++) {
        bg[i] = blank;
    }

    move_bkg(0u, 0u);
    SCX_REG = 0u;
    SCY_REG = 0u;
    set_bkg_tiles(0u, 0u, BG_DRAW_W, BG_DRAW_H, bg);
}

void battle_status_ui_runtime_draw_party_status_box(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 5u);
    jp_draw_bkg_frame(0u, 0u, 20u, 5u);
    jp_bkg_clear_area(1u, 1u, 18u, 3u);

    status_draw_member(0u, 1u);
    status_draw_member(1u, 7u);
    status_draw_member(2u, 14u);
}
