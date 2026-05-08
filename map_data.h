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

UINT8 map_data_collision16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED;
UINT8 map_data_object16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED;

#endif /* MAP_DATA_H_INCLUDE */
