#ifndef UI_ICONS_H
#define UI_ICONS_H

#include <gb/gb.h>

#define UI_ICON_TILE_BASE 244u
#define UI_ICON_ITEM      0u
#define UI_ICON_SWORD     1u
#define UI_ICON_STAFF     2u
#define UI_ICON_TOOL_WEAPON 3u
#define UI_ICON_BOW       4u
#define UI_ICON_ARMOR     5u
#define UI_ICON_CLOTH     6u
#define UI_ICON_ACCESSORY 7u

void ui_icons_load(void) BANKED;
UINT8 ui_icon_tile(UINT8 icon_id) BANKED;
UINT8 ui_icon_tile_for_item(UINT8 item_id) BANKED;
UINT8 ui_icon_tile_for_weapon_type(UINT8 weapon_type) BANKED;
void ui_put_icon(UINT8 x, UINT8 y, UINT8 icon_tile) BANKED;

#endif
