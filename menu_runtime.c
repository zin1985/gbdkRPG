#pragma bank 5

#include <gb/gb.h>
#include "menu_runtime.h"
#include "audio.h"
#include "jpfont.h"
#include "inventory.h"
#include "party_runtime.h"

BANKREF(menu_runtime_bank)

#define MENU_STATUS    0u
#define MENU_ITEM      1u
#define MENU_EQUIP     2u
#define MENU_SKILL     3u
#define MENU_OBJECTIVE 4u
#define MENU_COUNT     5u

static void menu_screen_clear(void) {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
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
    jp_put_bkg_text(1u, 16u, "G");
    menu_u16_to_dec(inventory_get_money(), buf);
    jp_put_bkg_text(3u, 16u, buf);
}

static void menu_put_cursor(UINT8 col, UINT8 row, UINT8 visible) {
    jp_put_bkg_text(col, row, visible ? ">" : " ");
}

static void menu_draw_static(void) {
    jp_draw_bkg_frame(0u, 0u, 10u, 10u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, "メニュー");
    jp_put_bkg_text(3u, 3u, "つよさ");
    jp_put_bkg_text(3u, 4u, "もちもの");
    jp_put_bkg_text(3u, 5u, "そうび");
    jp_put_bkg_text(3u, 6u, "とくぎ");
    jp_put_bkg_text(3u, 7u, "もくてき");
    menu_draw_money();
    jp_put_bkg_text(1u,17u, "A けってい B もどる");
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
    jp_put_bkg_text(1u, 1u, "もくてき");
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

    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();

        if (keys & J_UP) {
            if (cursor == 0u) cursor = MENU_COUNT - 1u;
            else cursor--;
            menu_draw_cursor(cursor);
        } else if (keys & J_DOWN) {
            cursor++;
            if (cursor >= MENU_COUNT) cursor = 0u;
            menu_draw_cursor(cursor);
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            if (cursor == MENU_STATUS) {
                party_menu_show_status_loop();
            } else if (cursor == MENU_ITEM) {
                inventory_menu_show_items_loop();
            } else if (cursor == MENU_EQUIP) {
                party_menu_show_equip_loop();
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
        }
    }
}
