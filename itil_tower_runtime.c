#pragma bank 16

#include <gb/gb.h>
#include "itil_tower_runtime.h"
#include "jpfont.h"
#include "audio.h"
#include "inventory.h"
#include "itil_quiz_bank.h"

BANKREF(itil_tower_runtime_bank)

#define ITIL_TOWER_MAX_FLOOR 100u

static UINT8 itil_floor;
static UINT8 itil_floor_answered;


static void itil_u8_to_dec3(UINT8 value, char *out) BANKED {
    UINT8 i;
    UINT8 h;
    UINT8 t;
    i = 0u;
    h = (UINT8)(value / 100u);
    t = (UINT8)((value / 10u) % 10u);
    if (h != 0u) out[i++] = (char)('0' + h);
    if (h != 0u || t != 0u) out[i++] = (char)('0' + t);
    out[i++] = (char)('0' + (UINT8)(value % 10u));
    out[i] = '\0';
}

static void itil_clear_ui(void) BANKED {
    DISPLAY_OFF;
    HIDE_WIN;
    HIDE_SPRITES;
    SHOW_BKG;
    move_bkg(0u, 0u);
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    DISPLAY_ON;
}

static void itil_wait_a(void) BANKED {
    audio_waitpad(J_A | J_B | J_START);
    audio_waitpadup();
    SHOW_SPRITES;
}

void itil_tower_start(void) BANKED {
    itil_floor = 1u;
    itil_floor_answered = 0u;
}

void itil_tower_next_floor(void) BANKED {
    if (itil_floor < ITIL_TOWER_MAX_FLOOR) {
        itil_floor++;
    }
    itil_floor_answered = 0u;
}

UINT8 itil_tower_get_floor(void) BANKED {
    if (itil_floor == 0u) itil_floor = 1u;
    return itil_floor;
}

UINT8 itil_tower_can_advance(void) BANKED {
    return itil_floor_answered;
}

void itil_tower_show_floor_banner(void) BANKED {
    char buf[4];
    itil_clear_ui();
    jp_draw_bkg_frame(3u, 6u, 14u, 5u);
    jp_put_bkg_text(5u, 7u, "アイティルの塔");
    itil_u8_to_dec3(itil_tower_get_floor(), buf);
    jp_put_bkg_text(7u, 9u, buf);
    jp_put_bkg_text(10u, 9u, "階");
    itil_wait_a();
}

void itil_tower_show_need_answer(void) BANKED {
    itil_clear_ui();
    jp_draw_bkg_frame(2u, 6u, 16u, 5u);
    jp_put_bkg_text(4u, 7u, "まず問題を");
    jp_put_bkg_text(4u, 8u, "解いてください");
    itil_wait_a();
}

void itil_tower_show_wrong_exit(void) BANKED {
    itil_clear_ui();
    jp_draw_bkg_frame(2u, 6u, 16u, 5u);
    jp_put_bkg_text(5u, 7u, "不正解！");
    jp_put_bkg_text(4u, 8u, "塔から退場です");
    itil_wait_a();
}

UINT8 itil_tower_ask_question(void) BANKED {
    UINT8 cursor;
    UINT8 keys;
    UINT8 idx;
    UINT8 quiz_count;

    if (itil_floor_answered) {
        itil_clear_ui();
        jp_draw_bkg_frame(2u, 6u, 16u, 5u);
        jp_put_bkg_text(4u, 7u, "この階は合格");
        jp_put_bkg_text(4u, 8u, "右の出口へ");
        itil_wait_a();
        return 1u;
    }

    quiz_count = itil_quiz_count();
    if (quiz_count == 0u) quiz_count = 1u;
    idx = (UINT8)((itil_tower_get_floor() - 1u) % quiz_count);
    cursor = 0u;

    while (1) {
        itil_clear_ui();
        jp_draw_bkg_frame(0u, 0u, 20u, 18u);
        jp_put_bkg_text(1u, 1u, "アイティル問題");
        itil_quiz_draw(idx, cursor);
        jp_put_bkg_text(1u,16u, "上下 Aで回答");

        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_START);
        audio_waitpadup();
        if (keys & J_UP) {
            if (cursor == 0u) cursor = 2u;
            else cursor--;
        } else if (keys & J_DOWN) {
            cursor++;
            if (cursor >= 3u) cursor = 0u;
        } else if (keys & J_A) {
            if (itil_quiz_is_correct(idx, cursor)) {
                itil_floor_answered = 1u;
                itil_clear_ui();
                jp_draw_bkg_frame(2u, 6u, 16u, 5u);
                jp_put_bkg_text(6u, 7u, "正解！");
                jp_put_bkg_text(4u, 8u, "右へ進めます");
                itil_wait_a();
                return 1u;
            }
            return 0u;
        }
    }
}

UINT8 itil_tower_grant_reward(UINT8 random_value) BANKED {
    UINT8 item_id;
    switch ((UINT8)(random_value % 3u)) {
        case 0u: item_id = ITEM_ITIL_SWORD; break;
        case 1u: item_id = ITEM_ITIL_ARMOR; break;
        default: item_id = ITEM_ITIL_CREST; break;
    }
    inventory_add(item_id, 1u);
    return item_id;
}

void itil_tower_show_reward(UINT8 item_id) BANKED {
    itil_clear_ui();
    jp_draw_bkg_frame(1u, 5u, 18u, 7u);
    jp_put_bkg_text(4u, 6u, "アイティルの塔 制覇");
    if (item_id == ITEM_ITIL_SWORD) jp_put_bkg_text(3u, 8u, "アイティルの剣を入手");
    else if (item_id == ITEM_ITIL_ARMOR) jp_put_bkg_text(3u, 8u, "アイティルの鎧を入手");
    else jp_put_bkg_text(3u, 8u, "アイティルの紋章を入手");
    itil_wait_a();
}
