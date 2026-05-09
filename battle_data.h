#ifndef BATTLE_DATA_H
#define BATTLE_DATA_H

#include <gb/gb.h>

#define BATTLE_DATA_MAX_ENEMIES 6u
#define BATTLE_NAME_LEN 18u

#define BATTLE_ENEMY_SIZE_S 0u /* 16x16, up to 6 */
#define BATTLE_ENEMY_SIZE_M 1u /* 32x32, up to 3 */
#define BATTLE_ENEMY_SIZE_L 2u /* 96x32, up to 1 */

typedef struct BattleEnemyData {
    char name[BATTLE_NAME_LEN];
    UINT16 max_hp;
    UINT16 hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 agility;
    UINT8 sprite_kind;
    UINT8 size_kind;
} BattleEnemyData;

void battle_data_load_random(UINT8 seed, BattleEnemyData *out, UINT8 *count) NONBANKED;

#endif
