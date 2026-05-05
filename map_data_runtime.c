#include <gb/gb.h>
#include "map_data.h"

BANKREF_EXTERN(map_data_bank)

extern const unsigned char collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char town_collision16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];
extern const unsigned char town_object16_map[MAP_DATA_HEIGHT][MAP_DATA_WIDTH];

UINT8 map_data_collision16_at(UINT8 area, UINT8 tx, UINT8 ty) NONBANKED {
    UINT8 save_bank;
    UINT8 value;

    if (tx >= MAP_DATA_WIDTH || ty >= MAP_DATA_HEIGHT) {
        return 1u;
    }

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(map_data_bank));

    if (area == MAP_DATA_AREA_TOWN) {
        value = town_collision16_map[ty][tx];
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

    if (area == MAP_DATA_AREA_TOWN) {
        value = town_object16_map[ty][tx];
    } else {
        value = object16_map[ty][tx];
    }

    SWITCH_ROM(save_bank);
    return value;
}
