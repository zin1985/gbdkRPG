#pragma bank 7

#include <gb/gb.h>
#include "battle_data.h"

BANKREF(battle_data_bank)

typedef struct BattleEnemyTemplate {
    const char *name;
    UINT16 max_hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 agility;
    UINT8 sprite_kind;
} BattleEnemyTemplate;

typedef struct BattleEncounterTemplate {
    UINT8 count;
    UINT8 enemy0;
    UINT8 enemy1;
    UINT8 enemy2;
} BattleEncounterTemplate;

static const char name_slime[] = "スライム";
static const char name_bat[]   = "こうもり";
static const char name_beast[] = "けもの";

static const BattleEnemyTemplate enemy_table[] = {
    {name_slime, 20u, 5u, 2u, 5u, 0u},
    {name_bat,   14u, 4u, 1u, 8u, 1u},
    {name_beast, 28u, 7u, 3u, 4u, 2u}
};

static const BattleEncounterTemplate encounter_table[] = {
    /* rpg081: every visible battle starts with three enemies. */
    {3u, 0u, 0u, 0u},
    {3u, 1u, 0u, 1u},
    {3u, 2u, 0u, 2u},
    {3u, 0u, 1u, 0u},
    {3u, 0u, 0u, 2u},
    {3u, 0u, 1u, 2u},
    {3u, 0u, 0u, 1u}
};

#define ENEMY_TABLE_COUNT ((UINT8)(sizeof(enemy_table) / sizeof(enemy_table[0])))
#define ENCOUNTER_TABLE_COUNT ((UINT8)(sizeof(encounter_table) / sizeof(encounter_table[0])))

UINT8 battle_data_encounter_count_bank(void) {
    return ENCOUNTER_TABLE_COUNT;
}

void battle_data_copy_name_bank(UINT8 enemy_id, char *dst, UINT8 max_len) {
    UINT8 si;
    UINT8 di;
    UINT8 step;
    UINT8 k;
    const unsigned char *src;

    if (enemy_id >= ENEMY_TABLE_COUNT) enemy_id = 0u;
    src = (const unsigned char *)enemy_table[enemy_id].name;

    if (max_len == 0u) return;

    si = 0u;
    di = 0u;
    while (src[si] != '\0') {
        if ((src[si] & 0x80u) == 0u) step = 1u;
        else if ((src[si] & 0xE0u) == 0xC0u) step = 2u;
        else if ((src[si] & 0xF0u) == 0xE0u) step = 3u;
        else step = 1u;

        if ((UINT8)(di + step) >= max_len) break;

        for (k = 0u; k < step; k++) {
            dst[di] = (char)src[si];
            di++;
            si++;
        }
    }
    dst[di] = '\0';
}

void battle_data_copy_enemy_bank(UINT8 enemy_id, BattleEnemyData *out) {
    if (enemy_id >= ENEMY_TABLE_COUNT) enemy_id = 0u;

    battle_data_copy_name_bank(enemy_id, out->name, BATTLE_NAME_LEN);
    out->max_hp = enemy_table[enemy_id].max_hp;
    out->hp = enemy_table[enemy_id].max_hp;
    out->attack = enemy_table[enemy_id].attack;
    out->defense = enemy_table[enemy_id].defense;
    out->agility = enemy_table[enemy_id].agility;
    out->sprite_kind = enemy_table[enemy_id].sprite_kind;
}

void battle_data_load_encounter_bank(UINT8 encounter_id, BattleEnemyData *out, UINT8 *count) {
    BattleEncounterTemplate enc;

    if (encounter_id >= ENCOUNTER_TABLE_COUNT) encounter_id = 0u;
    enc = encounter_table[encounter_id];

    *count = enc.count;
    if (*count == 0u) *count = 1u;
    if (*count > BATTLE_DATA_MAX_ENEMIES) *count = BATTLE_DATA_MAX_ENEMIES;

    battle_data_copy_enemy_bank(enc.enemy0, &out[0]);
    if (*count > 1u) {
        battle_data_copy_enemy_bank(enc.enemy1, &out[1]);
    }
    if (*count > 2u) {
        battle_data_copy_enemy_bank(enc.enemy2, &out[2]);
    }
}
