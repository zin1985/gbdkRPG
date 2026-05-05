#ifndef BATTLE_DATA_H
#define BATTLE_DATA_H

#include <gb/gb.h>

#define BATTLE_DATA_MAX_ENEMIES 2u
#define BATTLE_NAME_LEN 18u

typedef struct BattleEnemyData {
    char name[BATTLE_NAME_LEN];
    UINT8 max_hp;
    UINT8 hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 agility;
    UINT8 sprite_kind;
} BattleEnemyData;

void battle_data_load_random(UINT8 seed, BattleEnemyData *out, UINT8 *count) NONBANKED;

#endif
