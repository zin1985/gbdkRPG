#pragma bank 5

#include <gb/gb.h>
#include "field_overlay_runtime.h"
#include "jpfont.h"
#include "party_runtime.h"
#include "map_data.h"

BANKREF(field_overlay_runtime)

#define FIELD_MINIMAP_TILE_BASE 240u

static char num_buf[6];
static unsigned char minimap_tiles[4u * 16u];

static void mini_put_u16(UINT8 x, UINT8 y, UINT16 value) {
    UINT8 pos;
    UINT8 started;
    UINT16 div;

    pos = 0u;
    started = 0u;
    div = 100u;
    while (div > 0u) {
        UINT8 digit;
        digit = (UINT8)(value / div);
        value = (UINT16)(value % div);
        if (digit != 0u || started || div == 1u) {
            num_buf[pos++] = (char)('0' + digit);
            started = 1u;
        }
        div = (UINT16)(div / 10u);
    }
    num_buf[pos] = '\0';
    jp_put_bkg_text(x, y, num_buf);
}

static UINT8 minimap_level_for_kind(UINT8 kind) {
    if (kind == 0u) return 0u; /* floor: white */
    if (kind == 1u) return 3u; /* wall/building: black */
    if (kind == 2u) return 2u; /* rock/pit: dark gray */
    if (kind == 3u) return 1u; /* warp/town: light gray */
    if (kind == 4u) return 3u; /* treasure: black */
    if (kind == 5u) return 2u; /* hazard: dark gray */
    return 1u;
}

static void build_minimap_tiles(UINT8 area, UINT8 player_tx, UINT8 player_ty) {
    UINT8 tile;
    UINT8 row;
    UINT8 col;
    UINT8 map_x;
    UINT8 map_y;
    UINT8 lo;
    UINT8 hi;
    UINT8 level;
    UINT8 out;

    out = 0u;
    for (tile = 0u; tile < 4u; tile++) {
        UINT8 base_x;
        UINT8 base_y;
        base_x = (UINT8)((tile & 1u) * 8u);
        base_y = (UINT8)((tile >> 1u) * 8u);
        for (row = 0u; row < 8u; row++) {
            lo = 0u;
            hi = 0u;
            for (col = 0u; col < 8u; col++) {
                map_x = (UINT8)(base_x + col);
                map_y = (UINT8)(base_y + row);
                if (map_x == player_tx && map_y == player_ty) {
                    level = 3u;
                } else {
                    level = minimap_level_for_kind(map_data_object16_at(area, map_x, map_y));
                }
                lo = (UINT8)((lo << 1u) | (level & 1u));
                hi = (UINT8)((hi << 1u) | ((level >> 1u) & 1u));
            }
            minimap_tiles[out++] = lo;
            minimap_tiles[out++] = hi;
        }
    }
}

static UINT8 overlay_screen_bg_x(UINT8 camera_px) {
    UINT8 x;
    x = (UINT8)(((camera_px >> 3u) + 10u) & 31u);
    if (x > 22u) x = 10u;
    return x;
}

static UINT8 overlay_screen_bg_y(UINT8 camera_py) {
    UINT8 y;
    y = (UINT8)(((camera_py >> 3u) + 8u) & 31u);
    if (y > 22u) y = 8u;
    return y;
}

void field_overlay_runtime_show(UINT8 area, UINT8 player_tx, UINT8 player_ty, UINT8 camera_px, UINT8 camera_py) BANKED {
    UINT8 x;
    UINT8 y;
    unsigned char map_cells[4];

    x = overlay_screen_bg_x(camera_px);
    y = overlay_screen_bg_y(camera_py);

    jp_draw_bkg_frame(x, y, 10u, 10u);
    jp_bkg_clear_area((UINT8)(x + 1u), (UINT8)(y + 1u), 8u, 8u);

    jp_put_bkg_text((UINT8)(x + 1u), (UINT8)(y + 1u), "1H");
    mini_put_u16((UINT8)(x + 3u), (UINT8)(y + 1u), party_get_active_hp(0u));
    jp_put_bkg_text((UINT8)(x + 6u), (UINT8)(y + 1u), "M");
    mini_put_u16((UINT8)(x + 7u), (UINT8)(y + 1u), party_get_active_mp(0u));

    jp_put_bkg_text((UINT8)(x + 1u), (UINT8)(y + 2u), "2H");
    mini_put_u16((UINT8)(x + 3u), (UINT8)(y + 2u), party_get_active_hp(1u));
    jp_put_bkg_text((UINT8)(x + 6u), (UINT8)(y + 2u), "M");
    mini_put_u16((UINT8)(x + 7u), (UINT8)(y + 2u), party_get_active_mp(1u));

    jp_put_bkg_text((UINT8)(x + 1u), (UINT8)(y + 3u), "3H");
    mini_put_u16((UINT8)(x + 3u), (UINT8)(y + 3u), party_get_active_hp(2u));
    jp_put_bkg_text((UINT8)(x + 6u), (UINT8)(y + 3u), "M");
    mini_put_u16((UINT8)(x + 7u), (UINT8)(y + 3u), party_get_active_mp(2u));

    jp_put_bkg_text((UINT8)(x + 1u), (UINT8)(y + 5u), "MAP");
    build_minimap_tiles(area, player_tx, player_ty);
    set_bkg_data(FIELD_MINIMAP_TILE_BASE, 4u, minimap_tiles);
    map_cells[0] = FIELD_MINIMAP_TILE_BASE;
    map_cells[1] = FIELD_MINIMAP_TILE_BASE + 1u;
    map_cells[2] = FIELD_MINIMAP_TILE_BASE + 2u;
    map_cells[3] = FIELD_MINIMAP_TILE_BASE + 3u;
    set_bkg_tiles((UINT8)(x + 1u), (UINT8)(y + 6u), 2u, 2u, map_cells);
    jp_put_bkg_text((UINT8)(x + 4u), (UINT8)(y + 6u), "■=P");
}
