#ifndef INVENTORY_H
#define INVENTORY_H

#include <gb/gb.h>

#define INVENTORY_ITEM_MAX 64u
#define INVENTORY_STACK_MAX 99u

#define ITEM_NONE       0u
#define ITEM_HERB       1u
#define ITEM_POTION     2u
#define ITEM_CASTLE_KEY 3u

void inventory_clear(void);
UINT8 inventory_get_count(UINT8 item_id);
UINT8 inventory_has(UINT8 item_id, UINT8 amount);
UINT8 inventory_add(UINT8 item_id, UINT8 amount);
UINT8 inventory_remove(UINT8 item_id, UINT8 amount);
void inventory_copy_to(UINT8 *dst);
void inventory_copy_from(const UINT8 *src);

#endif
