#pragma bank 7

#include <gb/gb.h>
#include "inventory.h"


static UINT8 g_inventory_counts[INVENTORY_ITEM_MAX];

void inventory_clear(void) {
    UINT8 i;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        g_inventory_counts[i] = 0u;
    }
}

UINT8 inventory_get_count(UINT8 item_id) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return g_inventory_counts[item_id];
}

UINT8 inventory_has(UINT8 item_id, UINT8 amount) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return (g_inventory_counts[item_id] >= amount) ? 1u : 0u;
}

UINT8 inventory_add(UINT8 item_id, UINT8 amount) {
    UINT16 total;

    if (item_id == ITEM_NONE) return 0u;
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;

    total = (UINT16)g_inventory_counts[item_id] + (UINT16)amount;
    if (total > INVENTORY_STACK_MAX) {
        g_inventory_counts[item_id] = INVENTORY_STACK_MAX;
        return 0u;
    }

    g_inventory_counts[item_id] = (UINT8)total;
    return 1u;
}

UINT8 inventory_remove(UINT8 item_id, UINT8 amount) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;
    if (g_inventory_counts[item_id] < amount) return 0u;

    g_inventory_counts[item_id] = (UINT8)(g_inventory_counts[item_id] - amount);
    return 1u;
}

void inventory_copy_to(UINT8 *dst) {
    UINT8 i;

    if (dst == 0) return;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        dst[i] = g_inventory_counts[i];
    }
}

void inventory_copy_from(const UINT8 *src) {
    UINT8 i;

    if (src == 0) return;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        if (src[i] > INVENTORY_STACK_MAX) {
            g_inventory_counts[i] = INVENTORY_STACK_MAX;
        } else {
            g_inventory_counts[i] = src[i];
        }
    }
}
