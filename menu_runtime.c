#pragma bank 5

#include <gb/gb.h>
#include <gb/hardware.h>
#include "menu_runtime.h"
#include "audio.h"
#include "jpfont.h"
#include "menu_text_dict.h"
#include "inventory.h"
#include "party_runtime.h"
#include "equip_runtime.h"

BANKREF(menu_runtime_bank)

#ifndef VBK_TILES
#define VBK_TILES 0u
#endif
#ifndef VBK_ATTRIBUTES
#define VBK_ATTRIBUTES 1u
#endif

static UINT8 menu_attr_row[20u];

static void menu_attr_rect0(UINT8 x, UINT8 y, UINT8 w, UINT8 h) {
    UINT8 i;
    UINT8 yy;
    if (w == 0u || h == 0u) return;
    if (w > 20u) w = 20u;
    for (i = 0u; i < w; i++) menu_attr_row[i] = 0u;
    VBK_REG = VBK_ATTRIBUTES;
    for (yy = 0u; yy < h; yy++) {
        set_bkg_tiles(x, (UINT8)(y + yy), w, 1u, menu_attr_row);
    }
    VBK_REG = VBK_TILES;
}

#define MENU_STATUS    0u
#define MENU_ITEM      1u
#define MENU_EQUIP     2u
#define MENU_SKILL     3u
#define MENU_OBJECTIVE 4u
#define MENU_COUNT     5u

static void menu_screen_clear(void) {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
    menu_attr_rect0(0u, 0u, 20u, 18u);
}

static void menu_slide_out_up(void) {
    UINT8 i;
    for (i = 0u; i < 18u; i++) {
        audio_wait_vbl();
        move_bkg(0u, (UINT8)((UINT8)(i + 1u) * 8u));
    }
    audio_wait_vbl();
    move_bkg(0u, 0u);
}

static void menu_slide_in_down(void) {
    UINT8 i;
    move_bkg(0u, 144u);
    for (i = 18u; i > 0u; i--) {
        audio_wait_vbl();
        move_bkg(0u, (UINT8)((UINT8)(i - 1u) * 8u));
    }
    move_bkg(0u, 0u);
}


static void menu_u16_to_dec(UINT16 value, char *out) {
    UINT8 i;
    UINT8 started;
    UINT16 div;

    i = 0u;
    started = 0u;
    div = 10000u;
    while (div > 0u) {
        UINT8 digit = 0u;
        while (value >= div) {
            value = (UINT16)(value - div);
            digit++;
        }
        if (digit != 0u || started || div == 1u) {
            out[i++] = (char)('0' + digit);
            started = 1u;
        }
        div /= 10u;
    }
    out[i] = '\0';
}

static void menu_draw_money(void) {
    char buf[8];
    jp_put_bkg_text(1u, 16u, "おかね");
    menu_u16_to_dec(inventory_get_money(), buf);
    jp_put_bkg_text(8u, 16u, buf);
    jp_put_bkg_text(13u, 16u, "G");
}

static void menu_put_cursor(UINT8 col, UINT8 row, UINT8 visible) {
    jp_put_bkg_text(col, row, visible ? ">" : " ");
}

#define MENU_REPEAT_DELAY 12u
#define MENU_REPEAT_RATE   4u

static UINT8 menu_wait_keys(UINT8 mask) BANKED {
    static UINT8 last_keys;
    static UINT8 repeat_count;
    UINT8 keys;
    UINT8 pressed;
    UINT8 dirs;

    while (1) {
        keys = (UINT8)(joypad() & mask);
        pressed = (UINT8)(keys & (UINT8)~last_keys);
        if (pressed & (UINT8)(J_A | J_B | J_START)) {
            last_keys = keys;
            repeat_count = 0u;
            return (UINT8)(pressed & (UINT8)(J_A | J_B | J_START));
        }
        dirs = (UINT8)(keys & (UINT8)(J_UP | J_DOWN));
        if (dirs != 0u) {
            if (dirs != (UINT8)(last_keys & (UINT8)(J_UP | J_DOWN))) {
                last_keys = keys;
                repeat_count = 0u;
                return dirs;
            }
            if (repeat_count < MENU_REPEAT_DELAY) repeat_count++;
            else {
                repeat_count = (UINT8)(MENU_REPEAT_DELAY - MENU_REPEAT_RATE);
                last_keys = keys;
                return dirs;
            }
        } else {
            repeat_count = 0u;
        }
        last_keys = keys;
        audio_wait_vbl();
    }
}

static void menu_precache_destination(UINT8 cursor) BANKED {
    /* rpg266:
     * Do not render destination screens while the top menu is still visible.
     * The JP font system uses a shared dynamic BG tile cache; hidden Japanese
     * pre-rendering can evict glyph tiles that the visible menu is using,
     * causing mojibake or freezes when hovering items/equipment.
     * Destination screens now build after selection, then keep their own
     * safe cursor/difference updates.
     */
    (void)cursor;
}

static void menu_draw_static(void) {
    jp_draw_bkg_frame(0u, 0u, 10u, 10u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, menu_dict_message(MENU_DICT_MSG_MENU_TITLE));
    jp_put_bkg_text(3u, 3u, menu_dict_message(MENU_DICT_MSG_MENU_STATUS));
    jp_put_bkg_text(3u, 4u, menu_dict_message(MENU_DICT_MSG_ITEMS_TITLE));
    jp_put_bkg_text(3u, 5u, menu_dict_message(MENU_DICT_MSG_MENU_EQUIP));
    jp_put_bkg_text(3u, 6u, menu_dict_message(MENU_DICT_MSG_BATTLE_SKILL));
    jp_put_bkg_text(3u, 7u, menu_dict_message(MENU_DICT_MSG_MENU_OBJECTIVE));
    menu_draw_money();
}

static void menu_draw_cursor(UINT8 cursor) {
    menu_put_cursor(1u, 3u, (UINT8)(cursor == MENU_STATUS));
    menu_put_cursor(1u, 4u, (UINT8)(cursor == MENU_ITEM));
    menu_put_cursor(1u, 5u, (UINT8)(cursor == MENU_EQUIP));
    menu_put_cursor(1u, 6u, (UINT8)(cursor == MENU_SKILL));
    menu_put_cursor(1u, 7u, (UINT8)(cursor == MENU_OBJECTIVE));
}

static void menu_show_objective(UINT8 enemy_defeated) {
    menu_screen_clear();
    jp_draw_bkg_frame(0u, 0u, 20u, 15u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, menu_dict_message(MENU_DICT_MSG_MENU_OBJECTIVE));
    if (enemy_defeated) {
        jp_put_bkg_text(2u, 4u, "まもの とうばつずみ");
        jp_put_bkg_text(2u, 6u, "NPCに はなすと ふっかつ");
    } else {
        jp_put_bkg_text(2u, 4u, "まものを たおす");
        jp_put_bkg_text(2u, 6u, "つぎは まちと どうくつ");
    }
    jp_put_bkg_text(1u,16u, "A/B/START もどる");
}

void menu_runtime_open(UINT8 enemy_defeated) BANKED {
    UINT8 keys;
    UINT8 cursor = MENU_STATUS;

    menu_screen_clear();
    menu_draw_static();
    menu_draw_cursor(cursor);
    DISPLAY_ON;
    menu_precache_destination(cursor);

    while (1) {
        keys = menu_wait_keys(J_UP | J_DOWN | J_A | J_B | J_START);

        if (keys & J_UP) {
            if (cursor == 0u) cursor = MENU_COUNT - 1u;
            else cursor--;
            menu_draw_cursor(cursor);
            menu_precache_destination(cursor);
        } else if (keys & J_DOWN) {
            cursor++;
            if (cursor >= MENU_COUNT) cursor = 0u;
            menu_draw_cursor(cursor);
            menu_precache_destination(cursor);
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            menu_precache_destination(cursor);
            /* rpg271: no whole-window slide for classic RPG menu transitions. */
            if (cursor == MENU_STATUS) {
                party_menu_show_status_loop();
            } else if (cursor == MENU_ITEM) {
                inventory_menu_show_items_loop();
            } else if (cursor == MENU_EQUIP) {
                equip_runtime_show_loop();
            } else if (cursor == MENU_SKILL) {
                party_menu_show_heal_skill_loop();
            } else {
                menu_show_objective(enemy_defeated);
                audio_waitpad(J_A | J_B | J_START);
                audio_waitpadup();
            }
            menu_screen_clear();
            menu_draw_static();
            menu_draw_cursor(cursor);
            /* rpg271: destination/menu redraw is instant-buffered, no slide-back. */
        }
    }
}
