#ifndef MAP_DATA_H_INCLUDE
#define MAP_DATA_H_INCLUDE

#include <gb/gb.h>

/*
 * rpg058 map data bank split.
 * These constants intentionally mirror the current 16x16 map scaffold.
 */
#define MAP_DATA_WIDTH  16u
#define MAP_DATA_HEIGHT 16u

#define MAP_DATA_AREA_FIELD   0u
#define MAP_DATA_AREA_TOWN    1u
#define MAP_DATA_AREA_PORT    2u
#define MAP_DATA_AREA_DUNGEON 3u
#define MAP_DATA_AREA_RUINS   4u
#define MAP_DATA_AREA_TOWER   5u
#define MAP_DATA_AREA_FIELD_EAST 6u
#define MAP_DATA_AREA_CAVE_1 7u
#define MAP_DATA_AREA_CAVE_2 8u
#define MAP_DATA_AREA_CAVE_3 9u
#define MAP_DATA_AREA_CAVE_4 10u
#define MAP_DATA_AREA_EAST_TOWN 11u

UINT8 map_data_collision16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED;
UINT8 map_data_object16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED;

#endif /* MAP_DATA_H_INCLUDE */
