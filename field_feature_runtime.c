#pragma bank 5

#include <gb/gb.h>
#include "field_feature_runtime.h"
#include "party_runtime.h"
#include "audio.h"
#include "messages.h"

BANKREF(field_feature_runtime_bank)

UINT8 field_feature_map_event(UINT8 area, UINT8 tx, UINT8 ty) BANKED {
    if (area == AREA_FIELD) {
        if (tx == 14u && ty == 2u) return MAP_EVENT_TOWN;
        if (tx == 14u && ty == 14u) return MAP_EVENT_PORT_TOWN;
        if (tx == 1u && ty == 14u) return MAP_EVENT_DUNGEON;
        if (tx == 1u && ty == 2u) return MAP_EVENT_RUINS;
        if (tx == 8u && ty == 8u) return MAP_EVENT_HEAL_SPRING;
        if (tx == 8u && ty == 14u) return MAP_EVENT_ITIL_TOWER;
    } else if (area == AREA_TOWN) {
        if (tx == 2u && ty == 14u) return MAP_EVENT_FIELD_EXIT;
        /* rpg196: upper row shops.
         * Shops are placed on the blocked 7 tiles at y=2.  The actual talk
         * triggers are the front/left/right counter blocks at y=3 so the
         * player can talk through the counter without stepping into the shop.
         */
        if ((tx == 4u && ty == 2u) || (ty == 3u && (tx == 3u || tx == 4u || tx == 5u))) return MAP_EVENT_SHOP_INN;
        if ((tx == 8u && ty == 2u) || (ty == 3u && (tx == 7u || tx == 8u || tx == 9u))) return MAP_EVENT_SHOP_ITEM;
        if ((tx == 12u && ty == 2u) || (ty == 3u && (tx == 11u || tx == 12u || tx == 13u))) return MAP_EVENT_SHOP_EQUIP;
        if (tx == 8u && ty == 8u) return MAP_EVENT_SAVE_POINT;
        if (tx == 5u && ty == 8u) return MAP_EVENT_TOWN_VILLAGER;
        if (tx == 11u && ty == 8u) return MAP_EVENT_CHEST;
    } else if (area == AREA_PORT) {
        if (tx == 2u && ty == 14u) return MAP_EVENT_PORT_EXIT;
    } else if (area == AREA_TOWER) {
        if (tx == 14u && ty == 2u) return MAP_EVENT_ITIL_TOWER_NEXT;
        if (tx == 8u && ty == 8u) return MAP_EVENT_ITIL_TOWER_NPC;
        if (tx == 2u && ty == 14u) return MAP_EVENT_DUNGEON_EXIT;
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
    if (area == AREA_TOWER) return 0u;
    if (area == AREA_DUNGEON) return 34u;
    if (area == AREA_RUINS) return 28u;
    if (area == AREA_FIELD) return field_rate;
    return 0u;
}




void field_feature_activate_heal_spring_runtime(void) BANKED {
    party_heal_all_active();
    message_show(MSG_HEAL_SPRING);
}

UINT8 field_feature_music_track(UINT8 area) BANKED {
    if (area == AREA_TOWN || area == AREA_PORT) return AUDIO_TRACK_TOWN;
    if (area == AREA_DUNGEON || area == AREA_TOWER) return AUDIO_TRACK_DUNGEON;
    if (area == AREA_RUINS) return AUDIO_TRACK_RUINS;
    return AUDIO_TRACK_FIELD;
}

UINT8 field_feature_random_encounter_should_start_runtime(UINT8 area, UINT8 can_check, UINT8 tx, UINT8 ty, UINT8 *grace_steps, UINT8 *rand_seed_ptr, UINT8 field_rate) BANKED {
    UINT8 roll;

    if (area != AREA_FIELD && area != AREA_DUNGEON && area != AREA_RUINS && area != AREA_TOWER) return 0u;
    if (!can_check) return 0u;
    if (field_feature_map_event(area, tx, ty) != MAP_EVENT_NONE) return 0u;

    if (*grace_steps > 0u) {
        (*grace_steps)--;
        return 0u;
    }

    *rand_seed_ptr ^= DIV_REG;
    *rand_seed_ptr = (UINT8)((*rand_seed_ptr) * 17u + 29u);
    roll = *rand_seed_ptr;

    return (UINT8)(roll < field_feature_encounter_rate(area, field_rate));
}

UINT8 field_feature_chest_flag(UINT8 area) BANKED {
    if (area == AREA_RUINS) return FLAG_RUINS_CHEST;
    if (area == AREA_TOWN) return FLAG_TOWN_CHEST;
    return FLAG_DUNGEON_CHEST;
}
