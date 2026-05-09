#ifndef INVENTORY_H
#define INVENTORY_H

#include <gb/gb.h>

#define INVENTORY_ITEM_MAX 64u
#define INVENTORY_STACK_MAX 99u

#define ITEM_NONE          0u
#define ITEM_HERB          1u
#define ITEM_POTION        2u
#define ITEM_CASTLE_KEY    3u
#define ITEM_MANA_HERB     4u
#define ITEM_ANTIDOTE      5u

#define ITEM_WOOD_SWORD    10u
#define ITEM_IRON_SWORD    11u
#define ITEM_OAK_STAFF     12u
#define ITEM_MAGE_STAFF    13u
#define ITEM_SHORT_BOW     14u
#define ITEM_GLOVES        15u
#define ITEM_TOOL_KIT      16u

#define ITEM_CLOTH_ARMOR   24u
#define ITEM_LEATHER_ARMOR 25u
#define ITEM_IRON_ARMOR    26u

#define ITEM_CHARM         32u
#define ITEM_FEATHER       33u
#define ITEM_DEBUG_NO_ENCOUNT 34u

void inventory_clear(void) BANKED;
void inventory_seed_defaults(void) BANKED;
UINT8 inventory_get_count(UINT8 item_id) BANKED;
UINT8 inventory_has(UINT8 item_id, UINT8 amount) BANKED;
UINT8 inventory_add(UINT8 item_id, UINT8 amount) BANKED;
UINT8 inventory_remove(UINT8 item_id, UINT8 amount) BANKED;
void inventory_copy_to(UINT8 *dst) BANKED;
void inventory_copy_from(const UINT8 *src) BANKED;
void inventory_menu_show_items_loop(void) BANKED;

#endif
