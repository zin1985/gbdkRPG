#ifndef MENU_TEXT_DICT_H
#define MENU_TEXT_DICT_H

#include <gb/gb.h>

#define MENU_DICT_MSG_ITEM_HELP 1u
#define MENU_DICT_MSG_ITEM_HELP_KANJI 2u
#define MENU_DICT_MSG_EMPTY_USABLE 3u
#define MENU_DICT_MSG_TARGET 4u
#define MENU_DICT_MSG_NO_EFFECT 5u
#define MENU_DICT_MSG_ITEMS_TITLE 6u
#define MENU_DICT_MSG_EMPTY 7u
#define MENU_DICT_MSG_CANNOT_USE 8u
#define MENU_DICT_MSG_USED 9u
#define MENU_DICT_MSG_UPPED 10u
#define MENU_DICT_MSG_HEAL 11u
#define MENU_DICT_MSG_TARGET_NO_SPACE 12u

#define MENU_DICT_MSG_SHOP_ITEM 13u
#define MENU_DICT_MSG_SHOP_WEAPON 14u
#define MENU_DICT_MSG_SHOP_HELP 15u
#define MENU_DICT_MSG_SHOP_BOUGHT 16u
#define MENU_DICT_MSG_NO_MONEY 17u
#define MENU_DICT_MSG_INN_TITLE 18u
#define MENU_DICT_MSG_INN_ASK 19u
#define MENU_DICT_MSG_INN_HELP 20u
#define MENU_DICT_MSG_INN_HEALED 21u
#define MENU_DICT_MSG_BATTLE_ATTACK 22u
#define MENU_DICT_MSG_BATTLE_MAGIC 23u
#define MENU_DICT_MSG_BATTLE_SKILL 24u
#define MENU_DICT_MSG_BATTLE_DEFENSE 25u
#define MENU_DICT_MSG_BATTLE_ESCAPE 26u
#define MENU_DICT_MSG_MENU_TITLE 27u
#define MENU_DICT_MSG_MENU_STATUS 28u
#define MENU_DICT_MSG_MENU_EQUIP 29u
#define MENU_DICT_MSG_MENU_OBJECTIVE 30u
#define MENU_DICT_MSG_ITEM_KIND 31u

const char *menu_dict_item_name(UINT8 item_id) BANKED;
const char *menu_dict_item_short_name(UINT8 item_id) BANKED;
const char *menu_dict_message(UINT8 msg_id) BANKED;
void menu_dict_put_item_name(UINT8 x, UINT8 y, UINT8 width, UINT8 item_id) BANKED;
void menu_dict_put_item_short_name(UINT8 x, UINT8 y, UINT8 width, UINT8 item_id) BANKED;
void menu_dict_put_message(UINT8 x, UINT8 y, UINT8 width, UINT8 msg_id) BANKED;

#endif
