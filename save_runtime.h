#ifndef SAVE_RUNTIME_H
#define SAVE_RUNTIME_H

#include <gb/gb.h>
#include "inventory.h"
#include "party_runtime.h"

#define SAVE_SLOT_COUNT 3u
#define SAVE_EVENT_FLAG_COUNT 16u
#define SAVE_TITLE_NEW 0u
#define SAVE_TITLE_CONTINUE 1u

typedef struct SaveSnapshot {
    UINT8 area;
    UINT8 player_tx;
    UINT8 player_ty;
    UINT8 player_dir;
    UINT8 battle_enemy_rank;
    UINT8 pot_opened;
    UINT16 money;
    UINT8 event_flags[SAVE_EVENT_FLAG_COUNT];
    UINT8 inventory_counts[INVENTORY_ITEM_MAX];
    PartySaveState party;
} SaveSnapshot;

UINT8 save_runtime_title_load(SaveSnapshot *out) BANKED;
UINT8 save_runtime_save_select(const SaveSnapshot *snapshot) BANKED;
UINT8 save_runtime_slot_has_data(UINT8 slot) BANKED;
UINT8 save_runtime_load_slot(UINT8 slot, SaveSnapshot *out) BANKED;
UINT8 save_runtime_save_slot(UINT8 slot, const SaveSnapshot *snapshot) BANKED;

#endif
