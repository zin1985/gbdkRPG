#pragma bank 19

#include <gb/gb.h>
#include "field_map_render_runtime.h"
#include "field_feature_runtime.h"
#include "map_data.h"
#include "sprites.h"
#include "banked_graphics.h"

BANKREF(field_map_render_runtime_bank)
BANKREF_EXTERN(sprite_data_bank)

/*
 * rpg222:
 * The heavy field BG renderer was moved out of Bank 0.
 *
 * Keep in Bank 0:
 * - player/camera/OAM state
 * - main loop
 * - tiny draw_object_map() bridge
 *
 * Move here:
 * - 32x32 BG buffer construction
 * - object-kind -> metatile mapping
 * - area-specific overlay tile loading
 *
 * Future map growth rule:
 * - Add new object kind rendering here or in further banked render tables.
 * - Do not grow main.c draw logic.
 */

#define FIELD_MAP16_W 16u
#define FIELD_MAP16_H 16u
#define MAP16_W FIELD_MAP16_W
#define MAP16_H FIELD_MAP16_H

#define BG_DRAW_W 32u
#define BG_DRAW_H 32u
#define DRAW_MAP16_W MAP16_W
#define DRAW_MAP16_H MAP16_H

#define MAP_TILE_BASE    96u
#define MAP_TILE_FLOOR   (MAP_TILE_BASE + 0u)
#define MAP_TILE_WALL_TL (MAP_TILE_BASE + 1u)
#define MAP_TILE_WALL_TR (MAP_TILE_BASE + 2u)
#define MAP_TILE_WALL_BL (MAP_TILE_BASE + 3u)
#define MAP_TILE_WALL_BR (MAP_TILE_BASE + 4u)
#define MAP_TILE_CAP_TL  (MAP_TILE_BASE + 5u)
#define MAP_TILE_CAP_TR  (MAP_TILE_BASE + 6u)
#define MAP_TILE_CAP_BL  (MAP_TILE_BASE + 7u)
#define MAP_TILE_CAP_BR  (MAP_TILE_BASE + 8u)
#define MAP_TILE_TOWN_TL (MAP_TILE_BASE + 9u)
#define MAP_TILE_TOWN_TR (MAP_TILE_BASE + 10u)
#define MAP_TILE_TOWN_BL (MAP_TILE_BASE + 11u)
#define MAP_TILE_TOWN_BR (MAP_TILE_BASE + 12u)
#define MAP_TILE_FOREST_TL (MAP_TILE_BASE + 13u)
#define MAP_TILE_FOREST_TR (MAP_TILE_BASE + 14u)
#define MAP_TILE_FOREST_BL (MAP_TILE_BASE + 15u)
#define MAP_TILE_FOREST_BR (MAP_TILE_BASE + 16u)
#define MAP_TILE_DUNGEON_WALL_TL (MAP_TILE_BASE + 17u)
#define MAP_TILE_DUNGEON_WALL_TR (MAP_TILE_BASE + 18u)
#define MAP_TILE_DUNGEON_WALL_BL (MAP_TILE_BASE + 19u)
#define MAP_TILE_DUNGEON_WALL_BR (MAP_TILE_BASE + 20u)
#define MAP_TILE_DUNGEON_PIT_TL  (MAP_TILE_BASE + 21u)
#define MAP_TILE_DUNGEON_PIT_TR  (MAP_TILE_BASE + 22u)
#define MAP_TILE_DUNGEON_PIT_BL  (MAP_TILE_BASE + 23u)
#define MAP_TILE_DUNGEON_PIT_BR  (MAP_TILE_BASE + 24u)
#define MAP_TILE_CHEST_TL (MAP_TILE_BASE + 25u)
#define MAP_TILE_CHEST_TR (MAP_TILE_BASE + 26u)
#define MAP_TILE_CHEST_BL (MAP_TILE_BASE + 27u)
#define MAP_TILE_CHEST_BR (MAP_TILE_BASE + 28u)

#define FOREST_BG_X 6u
#define FOREST_BG_Y 5u

static UINT8 renderer_area_dangerous(UINT8 area) BANKED {
    return (UINT8)(area == AREA_DUNGEON || area == AREA_RUINS || area == AREA_TOWER || area == AREA_CAVE_1 || area == AREA_CAVE_2 || area == AREA_CAVE_3 || area == AREA_CAVE_4);
}

static void renderer_select_metatile(UINT8 area, UINT8 kind, UINT8 *tl, UINT8 *tr, UINT8 *bl, UINT8 *br) BANKED {
    UINT8 area_dangerous;

    area_dangerous = renderer_area_dangerous(area);

    if (kind == 1u) {
        if (area_dangerous) {
            *tl = MAP_TILE_DUNGEON_WALL_TL;
            *tr = MAP_TILE_DUNGEON_WALL_TR;
            *bl = MAP_TILE_DUNGEON_WALL_BL;
            *br = MAP_TILE_DUNGEON_WALL_BR;
        } else {
            *tl = MAP_TILE_WALL_TL;
            *tr = MAP_TILE_WALL_TR;
            *bl = MAP_TILE_WALL_BL;
            *br = MAP_TILE_WALL_BR;
        }
    } else if (kind == 2u) {
        if (area == AREA_PORT || area_dangerous) {
            *tl = MAP_TILE_DUNGEON_PIT_TL;
            *tr = MAP_TILE_DUNGEON_PIT_TR;
            *bl = MAP_TILE_DUNGEON_PIT_BL;
            *br = MAP_TILE_DUNGEON_PIT_BR;
        } else {
            *tl = MAP_TILE_CAP_TL;
            *tr = MAP_TILE_CAP_TR;
            *bl = MAP_TILE_CAP_BL;
            *br = MAP_TILE_CAP_BR;
        }
    } else if (kind == 5u) {
        if (area == AREA_TOWN || area_dangerous) {
            *tl = MAP_TILE_DUNGEON_PIT_TL;
            *tr = MAP_TILE_DUNGEON_PIT_TR;
            *bl = MAP_TILE_DUNGEON_PIT_BL;
            *br = MAP_TILE_DUNGEON_PIT_BR;
        } else {
            *tl = MAP_TILE_CAP_TL;
            *tr = MAP_TILE_CAP_TR;
            *bl = MAP_TILE_CAP_BL;
            *br = MAP_TILE_CAP_BR;
        }
    } else if (kind == 3u) {
        *tl = MAP_TILE_TOWN_TL;
        *tr = MAP_TILE_TOWN_TR;
        *bl = MAP_TILE_TOWN_BL;
        *br = MAP_TILE_TOWN_BR;
    } else if (kind == 4u) {
        *tl = MAP_TILE_CHEST_TL;
        *tr = MAP_TILE_CHEST_TR;
        *bl = MAP_TILE_CHEST_BL;
        *br = MAP_TILE_CHEST_BR;
    } else if (kind == 6u) {
        *tl = MAP_TILE_DUNGEON_PIT_TL;
        *tr = MAP_TILE_DUNGEON_PIT_TR;
        *bl = MAP_TILE_DUNGEON_PIT_BL;
        *br = MAP_TILE_DUNGEON_PIT_BR;
    } else if (kind == 7u) {
        *tl = MAP_TILE_FOREST_TL;
        *tr = MAP_TILE_FOREST_TR;
        *bl = MAP_TILE_FOREST_BL;
        *br = MAP_TILE_FOREST_BR;
    } else if (kind == 8u) {
        *tl = MAP_TILE_DUNGEON_WALL_TL;
        *tr = MAP_TILE_DUNGEON_WALL_TR;
        *bl = MAP_TILE_DUNGEON_WALL_BL;
        *br = MAP_TILE_DUNGEON_WALL_BR;
    } else {
        *tl = MAP_TILE_FLOOR;
        *tr = MAP_TILE_FLOOR;
        *bl = MAP_TILE_FLOOR;
        *br = MAP_TILE_FLOOR;
    }
}

void field_map_render_runtime_draw(UINT8 area) BANKED {
    UINT8 x;
    UINT8 y;
    static UINT8 bg[BG_DRAW_W * BG_DRAW_H];
    UINT8 sx;
    UINT8 sy;
    UINT8 kind;
    UINT8 tl;
    UINT8 tr;
    UINT8 bl;
    UINT8 br;
    UINT16 bg_i;
    UINT16 top_left_i;

    for (y = 0u; y < BG_DRAW_H; y++) {
        for (x = 0u; x < BG_DRAW_W; x++) {
            bg_i = (UINT16)y * (UINT16)BG_DRAW_W + (UINT16)x;
            bg[bg_i] = MAP_TILE_FLOOR;
        }
    }

    for (y = 0u; y < DRAW_MAP16_H; y++) {
        for (x = 0u; x < DRAW_MAP16_W; x++) {
            kind = map_data_object16_at(area, x, y);

            sx = (UINT8)(x * 2u);
            sy = (UINT8)(y * 2u);

            renderer_select_metatile(area, kind, &tl, &tr, &bl, &br);

            top_left_i = (UINT16)sy * (UINT16)BG_DRAW_W + (UINT16)sx;
            bg[top_left_i] = tl;
            bg[top_left_i + 1u] = tr;
            bg[top_left_i + (UINT16)BG_DRAW_W] = bl;
            bg[top_left_i + (UINT16)BG_DRAW_W + 1u] = br;
        }
    }

    if (area == AREA_FIELD) {
        sx = (UINT8)(FOREST_BG_X * 2u);
        sy = (UINT8)(FOREST_BG_Y * 2u);
        top_left_i = (UINT16)sy * (UINT16)BG_DRAW_W + (UINT16)sx;
        bg[top_left_i] = MAP_TILE_FOREST_TL;
        bg[top_left_i + 1u] = MAP_TILE_FOREST_TR;
        bg[top_left_i + (UINT16)BG_DRAW_W] = MAP_TILE_FOREST_BL;
        bg[top_left_i + (UINT16)BG_DRAW_W + 1u] = MAP_TILE_FOREST_BR;
    }

    set_banked_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles, BANK(sprite_data_bank));

    if (area == AREA_PORT || area == AREA_FIELD || area == AREA_FIELD_EAST) {
        map_load_port_overlay_tiles(MAP_TILE_DUNGEON_PIT_TL, MAP_TILE_CHEST_TL);
        map_load_pot_overlay_tiles(MAP_TILE_FOREST_TL);
    } else if (area == AREA_TOWN || area == AREA_EAST_TOWN) {
        map_load_town_sign_overlay_tiles(MAP_TILE_CHEST_TL, MAP_TILE_DUNGEON_PIT_TL);
        map_load_town_inn_sign_overlay_tiles(MAP_TILE_DUNGEON_WALL_TL);
        map_load_pot_overlay_tiles(MAP_TILE_FOREST_TL);
    }

    set_bkg_tiles(0u, 0u, BG_DRAW_W, BG_DRAW_H, bg);
}
