#pragma bank 10

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
    UINT8 size_kind;
} BattleEnemyTemplate;

typedef struct BattleEncounterTemplate {
    UINT8 count;
    UINT8 enemy_ids[BATTLE_DATA_MAX_ENEMIES];
} BattleEncounterTemplate;

static const char name_bit[]   = "ビット";
static const char name_beast[] = "けもの";
static const char name_warmachine[] = "ウォーマシン";
static const char name_golem[] = "ゴーレム";
static const char name_cave_ogre[] = "どうくつオーガ";
static const char name_sea_wraith[] = "うみぼうず";
static const char name_cave_lord[] = "どうくつのぬし";

static const BattleEnemyTemplate enemy_table[] = {
    /* name,            hp,  atk, def, agi, art, size */
    /* S-size kind 0 = ビット */
    {name_bit,          16u, 4u, 1u, 8u, 0u, BATTLE_ENEMY_SIZE_S},
    {name_bit,          16u, 4u, 1u, 8u, 0u, BATTLE_ENEMY_SIZE_S},
    {name_beast,        28u, 7u, 3u, 4u, 2u, BATTLE_ENEMY_SIZE_M},
    {name_warmachine,   36u, 9u, 4u, 5u, 3u, BATTLE_ENEMY_SIZE_M},
    {name_golem,        88u, 12u, 8u, 2u, 4u, BATTLE_ENEMY_SIZE_L},
    {name_warmachine,   96u, 14u, 9u, 3u, 5u, BATTLE_ENEMY_SIZE_L},
    {name_cave_ogre,    70u, 16u, 8u, 5u, 3u, BATTLE_ENEMY_SIZE_M},
    {name_sea_wraith,   64u, 15u, 7u, 7u, 2u, BATTLE_ENEMY_SIZE_M},
    {name_cave_lord,   180u, 24u, 14u, 6u, 5u, BATTLE_ENEMY_SIZE_L}
};

static const BattleEncounterTemplate encounter_table[] = {
    /* rpg150: encounter formations are organized by monster size.
     * S = 16x16 up to 6, M = 32x32 up to 3, L = 96x32 one body.
     * Empty slots use 0u because count controls the active range.
     */
    {6u, {0u, 0u, 0u, 0u, 0u, 0u}}, /* Bit S x6 */
    {4u, {0u, 0u, 0u, 0u, 0u, 0u}}, /* Bit S x4 */
    {3u, {2u, 3u, 2u, 0u, 0u, 0u}}, /* M x3 */
    {2u, {2u, 3u, 0u, 0u, 0u, 0u}}, /* M x2 */
    {1u, {4u, 0u, 0u, 0u, 0u, 0u}}, /* L golem x1 */
    {1u, {5u, 0u, 0u, 0u, 0u, 0u}}, /* L warmachine x1 */
    {3u, {0u, 0u, 0u, 0u, 0u, 0u}}, /* Bit S x3 */
    {1u, {3u, 0u, 0u, 0u, 0u, 0u}}, /* M x1 */
    {2u, {6u, 7u, 0u, 0u, 0u, 0u}}, /* strong cave pair */
    {3u, {6u, 3u, 7u, 0u, 0u, 0u}}, /* strong cave trio */
    {1u, {8u, 0u, 0u, 0u, 0u, 0u}}  /* cave boss */
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
    out->size_kind = enemy_table[enemy_id].size_kind;
}

void battle_data_load_encounter_bank(UINT8 encounter_id, BattleEnemyData *out, UINT8 *count) {
    BattleEncounterTemplate enc;

    if (encounter_id >= ENCOUNTER_TABLE_COUNT) encounter_id = 0u;
    enc = encounter_table[encounter_id];

    *count = enc.count;
    if (*count == 0u) *count = 1u;
    if (*count > BATTLE_DATA_MAX_ENEMIES) *count = BATTLE_DATA_MAX_ENEMIES;

    {
        UINT8 i;
        for (i = 0u; i < *count; i++) {
            battle_data_copy_enemy_bank(enc.enemy_ids[i], &out[i]);
        }
    }
}

void battle_data_load_boss_bank(BattleEnemyData *out, UINT8 *count) {
    *count = 1u;
    battle_data_copy_enemy_bank(8u, &out[0]);
}
