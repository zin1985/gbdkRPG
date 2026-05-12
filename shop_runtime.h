#ifndef SHOP_RUNTIME_H
#define SHOP_RUNTIME_H

#include <gb/gb.h>

#define SHOP_KIND_INN    1u
#define SHOP_KIND_ITEM   2u
#define SHOP_KIND_EQUIP  3u
#define SHOP_KIND_SAVE   4u
#define SHOP_KIND_ITEM_STRONG 5u
#define SHOP_KIND_EQUIP_STRONG 6u

void shop_runtime_open(UINT8 shop_kind) BANKED;
UINT8 shop_runtime_handle_event(UINT8 event_id) BANKED;
UINT8 shop_runtime_get_pot_opened(void) BANKED;
void shop_runtime_set_pot_opened(UINT8 opened) BANKED;

#endif
