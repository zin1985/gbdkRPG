#ifndef FIELD_FEATURE_RUNTIME_H
#define FIELD_FEATURE_RUNTIME_H

#include <gb/gb.h>

#define MAP_EVENT_NONE 0u
#define MAP_EVENT_TOWN 1u
#define MAP_EVENT_DUNGEON 2u
#define MAP_EVENT_FIELD_EXIT 3u
#define MAP_EVENT_PORT_TOWN 4u
#define MAP_EVENT_PORT_EXIT 5u
#define MAP_EVENT_DUNGEON_EXIT 6u
#define MAP_EVENT_RUINS 7u
#define MAP_EVENT_RUINS_EXIT 8u
#define MAP_EVENT_CHEST 9u
#define MAP_EVENT_RUIN_LORE 10u
#define MAP_EVENT_HEAL_SPRING 11u
#define MAP_EVENT_SHOP_INN 12u
#define MAP_EVENT_SHOP_ITEM 13u
#define MAP_EVENT_SHOP_EQUIP 14u
#define MAP_EVENT_SAVE_POINT 15u
#define MAP_EVENT_POT 16u
#define MAP_EVENT_TOWN_VILLAGER 17u
#define MAP_EVENT_ITIL_TOWER 18u
#define MAP_EVENT_ITIL_TOWER_NEXT 19u
#define MAP_EVENT_ITIL_TOWER_NPC 20u

#define AREA_FIELD 0u
#define AREA_TOWN 1u
#define AREA_PORT 2u
#define AREA_DUNGEON 3u
#define AREA_RUINS 4u
#define AREA_TOWER 5u

#define FLAG_DUNGEON_CHEST 20u
#define FLAG_RUINS_CHEST 21u
#define FLAG_TOWN_CHEST 22u
#define FLAG_TOWN_POT 23u

UINT8 field_feature_map_event(UINT8 area, UINT8 tx, UINT8 ty) BANKED;
UINT8 field_feature_encounter_rate(UINT8 area, UINT8 field_rate) BANKED;
UINT8 field_feature_chest_flag(UINT8 area) BANKED;

UINT8 field_feature_random_encounter_should_start_runtime(UINT8 area, UINT8 can_check, UINT8 tx, UINT8 ty, UINT8 *grace_steps, UINT8 *rand_seed_ptr, UINT8 field_rate) BANKED;

UINT8 field_feature_music_track(UINT8 area) BANKED;
void field_feature_activate_heal_spring_runtime(void) BANKED;

#endif
