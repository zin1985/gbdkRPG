#include <gb/gb.h>
#include "map_data.h"

BANKREF_EXTERN(map_data_bank)

extern const unsigned char collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char town_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char town_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char port_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char port_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char dungeon_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char dungeon_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char ruins_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char ruins_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char field_west_complex_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char field_west_complex_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char field_east_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char field_east_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave1_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave1_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave2_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave2_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave3_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave3_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave4_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char cave4_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char east_town_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char east_town_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];

UINT8 map_data_collision16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED {
    UINT8 save_bank;
    UINT8 value;

    if (tx >= MAP_DATA_WIDTH || ty >= MAP_DATA_HEIGHT) {
        return 1u;
    }

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(map_data_bank));

    if (area == MAP_DATA_AREA_FIELD) {
        value = field_west_complex_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_FIELD_EAST) {
        value = field_east_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_EAST_TOWN) {
        value = east_town_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_1) {
        value = cave1_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_2) {
        value = cave2_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_3) {
        value = cave3_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_4) {
        value = cave4_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_TOWN) {
        value = town_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_PORT) {
        value = port_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_DUNGEON || area == MAP_DATA_AREA_TOWER) {
        value = dungeon_collision16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_RUINS) {
        value = ruins_collision16_map[ty][tx];
    } else {
        value = collision16_map[ty][tx];
    }

    SWITCH_ROM(save_bank);
    return value;
}

UINT8 map_data_object16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED {
    UINT8 save_bank;
    UINT8 value;

    if (tx >= MAP_DATA_WIDTH || ty >= MAP_DATA_HEIGHT) {
        return 1u;
    }

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(map_data_bank));

    if (area == MAP_DATA_AREA_FIELD) {
        value = field_west_complex_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_FIELD_EAST) {
        value = field_east_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_EAST_TOWN) {
        value = east_town_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_1) {
        value = cave1_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_2) {
        value = cave2_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_3) {
        value = cave3_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_CAVE_4) {
        value = cave4_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_TOWN) {
        value = town_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_PORT) {
        value = port_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_DUNGEON || area == MAP_DATA_AREA_TOWER) {
        value = dungeon_object16_map[ty][tx];
    } else if (area == MAP_DATA_AREA_RUINS) {
        value = ruins_object16_map[ty][tx];
    } else {
        value = object16_map[ty][tx];
    }

    SWITCH_ROM(save_bank);
    return value;
}
