#ifndef PARTY_ITEM_RUNTIME_H
#define PARTY_ITEM_RUNTIME_H

#include <gb/gb.h>

UINT8 party_item_runtime_apply(UINT8 item_id,
                               UINT16 *hp, UINT16 max_hp,
                               UINT16 *mp, UINT16 max_mp,
                               UINT8 *status_flags,
                               UINT8 *defense,
                               UINT8 *attack,
                               UINT8 *agility,
                               UINT8 *focus,
                               UINT8 *morale,
                               UINT8 *last_amount,
                               UINT8 *last_kind) BANKED;

#endif
