#pragma bank 22

#include <gb/gb.h>
#include "bank_ids.h"
#include "battle_command_ui_runtime.h"
#include "jpfont.h"
#include "menu_text_dict.h"

BANKREF(battle_command_ui_runtime)

#define BATTLE_CURSOR_SPRITE 30u

#define CMD_ATTACK 0u
#define CMD_MAGIC  1u
#define CMD_SKILL  2u
#define CMD_ITEM   3u
#define CMD_DEFEND 4u
#define CMD_RUN    5u

void battle_command_ui_runtime_draw(void) BANKED {
    jp_draw_bkg_frame(9u, 13u, 11u, 5u);
    jp_put_bkg_text(11u, 14u, menu_dict_message(MENU_DICT_MSG_BATTLE_ATTACK));
    jp_put_bkg_text(16u, 14u, menu_dict_message(MENU_DICT_MSG_BATTLE_MAGIC));
    jp_put_bkg_text(11u, 15u, menu_dict_message(MENU_DICT_MSG_BATTLE_SKILL));
    jp_put_bkg_text(16u, 15u, menu_dict_message(MENU_DICT_MSG_ITEM_KIND));
    jp_put_bkg_text(11u, 16u, menu_dict_message(MENU_DICT_MSG_BATTLE_DEFENSE));
    jp_put_bkg_text(16u, 16u, menu_dict_message(MENU_DICT_MSG_BATTLE_ESCAPE));
}

void battle_command_ui_runtime_move_cursor(UINT8 menu_index) BANKED {
    UINT8 col;
    UINT8 row;

    move_sprite(BATTLE_CURSOR_SPRITE, 0u, 0u);
    jp_put_bkg_text(10u, 14u, " ");
    jp_put_bkg_text(15u, 14u, " ");
    jp_put_bkg_text(10u, 15u, " ");
    jp_put_bkg_text(15u, 15u, " ");
    jp_put_bkg_text(10u, 16u, " ");
    jp_put_bkg_text(15u, 16u, " ");

    col = (menu_index == CMD_MAGIC || menu_index == CMD_ITEM || menu_index == CMD_RUN) ? 15u : 10u;
    if (menu_index == CMD_ATTACK || menu_index == CMD_MAGIC) row = 14u;
    else if (menu_index == CMD_SKILL || menu_index == CMD_ITEM) row = 15u;
    else row = 16u;
    jp_put_bkg_text(col, row, ">");
}
