#pragma bank 5

#include <gb/gb.h>
#include "field_feature_runtime.h"
#include "party_runtime.h"

BANKREF(field_feature_runtime_bank)

UINT8 field_feature_map_event(UINT8 area, UINT8 tx, UINT8 ty) BANKED {
    if (area == AREA_FIELD) {
        if (tx == 14u && ty == 2u) return MAP_EVENT_TOWN;
        if (tx == 14u && ty == 14u) return MAP_EVENT_PORT_TOWN;
        if (tx == 1u && ty == 14u) return MAP_EVENT_DUNGEON;
        if (tx == 1u && ty == 2u) return MAP_EVENT_RUINS;
        if (tx == 8u && ty == 8u) return MAP_EVENT_HEAL_SPRING;
    } else if (area == AREA_TOWN) {
        if (tx == 2u && ty == 14u) return MAP_EVENT_FIELD_EXIT;
    } else if (area == AREA_PORT) {
        if (tx == 2u && ty == 14u) return MAP_EVENT_PORT_EXIT;
    } else if (area == AREA_DUNGEON) {
        if (tx == 2u && ty == 14u) return MAP_EVENT_DUNGEON_EXIT;
        if (tx == 8u && ty == 6u) return MAP_EVENT_CHEST;
    } else if (area == AREA_RUINS) {
        if (tx == 13u && ty == 14u) return MAP_EVENT_RUINS_EXIT;
        if (tx == 13u && ty == 2u) return MAP_EVENT_CHEST;
        if (tx == 1u && ty == 2u) return MAP_EVENT_RUIN_LORE;
    }
    return MAP_EVENT_NONE;
}

UINT8 field_feature_encounter_rate(UINT8 area, UINT8 field_rate) BANKED {
    if (party_debug_no_encounter_accessory_equipped()) return 0u;
    if (area == AREA_DUNGEON) return 34u;
    if (area == AREA_RUINS) return 28u;
    if (area == AREA_FIELD) return field_rate;
    return 0u;
}

UINT8 field_feature_chest_flag(UINT8 area) BANKED {
    return (area == AREA_RUINS) ? FLAG_RUINS_CHEST : FLAG_DUNGEON_CHEST;
}
