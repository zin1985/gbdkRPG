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
#define MAP_EVENT_EAST_TOWN 21u
#define MAP_EVENT_EAST_TOWN_EXIT 22u
#define MAP_EVENT_CAVE_DOWN 23u
#define MAP_EVENT_CAVE_UP 24u
#define MAP_EVENT_CAVE_TO_EAST 25u
#define MAP_EVENT_CAVE_BOSS 26u
#define MAP_EVENT_SHOP_ITEM_STRONG 27u
#define MAP_EVENT_SHOP_EQUIP_STRONG 28u
#define MAP_EVENT_MIMIC_CHEST 29u
#define MAP_EVENT_PLANT_EYE_CHEST 30u

#define AREA_FIELD 0u
#define AREA_TOWN 1u
#define AREA_PORT 2u
#define AREA_DUNGEON 3u
#define AREA_RUINS 4u
#define AREA_TOWER 5u
#define AREA_FIELD_EAST 6u
#define AREA_CAVE_1 7u
#define AREA_CAVE_2 8u
#define AREA_CAVE_3 9u
#define AREA_CAVE_4 10u
#define AREA_EAST_TOWN 11u

#define FLAG_DUNGEON_CHEST 20u
#define FLAG_RUINS_CHEST 21u
#define FLAG_TOWN_CHEST 22u
#define FLAG_TOWN_POT 23u
#define FLAG_CAVE_CHEST 24u
#define FLAG_CAVE_BOSS 25u
#define FLAG_MIMIC_CHEST 26u

UINT8 field_feature_map_event(UINT8 area, UINT8 tx, UINT8 ty) BANKED;
UINT8 field_feature_encounter_rate(UINT8 area, UINT8 field_rate) BANKED;
UINT8 field_feature_chest_flag(UINT8 area) BANKED;

UINT8 field_feature_random_encounter_should_start_runtime(UINT8 area, UINT8 can_check, UINT8 tx, UINT8 ty, UINT8 *grace_steps, UINT8 *rand_seed_ptr, UINT8 field_rate) BANKED;

UINT8 field_feature_music_track(UINT8 area) BANKED;
void field_feature_activate_heal_spring_runtime(void) BANKED;

#endif
