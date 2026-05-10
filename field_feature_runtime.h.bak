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

#define AREA_FIELD 0u
#define AREA_TOWN 1u
#define AREA_PORT 2u
#define AREA_DUNGEON 3u
#define AREA_RUINS 4u

#define FLAG_DUNGEON_CHEST 20u
#define FLAG_RUINS_CHEST 21u

UINT8 field_feature_map_event(UINT8 area, UINT8 tx, UINT8 ty) BANKED;
UINT8 field_feature_encounter_rate(UINT8 area, UINT8 field_rate) BANKED;
UINT8 field_feature_chest_flag(UINT8 area) BANKED;

#endif
