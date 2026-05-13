#pragma bank 20

#include <gb/gb.h>
#include "battle_skill_runtime.h"
#include "skill_defs.h"

BANKREF(battle_skill_meta_runtime)

typedef struct SkillMeta {
    UINT8 kind;
    UINT8 target;
    UINT8 mp_cost;
    UINT8 power;
    UINT8 accuracy;
    UINT8 flags;
} SkillMeta;

#define TARGET_ENEMY     0u
#define TARGET_SELF      1u
#define TARGET_ALLY      2u
#define TARGET_ALL_ENEMY 3u
#define TARGET_ALL_ALLY  4u

#define SKILL_FLAG_FIELD_OK   0x01u
#define SKILL_FLAG_AI_OK      0x02u
#define SKILL_FLAG_IGNORE_DEF 0x04u

static const SkillMeta skill_meta_table[SKILL_MAX] = {
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 0u, 0u, 100u, 0u},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 2u, 4u, 95u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_HEAL, TARGET_ALLY, 4u, 6u, 100u, SKILL_FLAG_FIELD_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 3u, 6u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 3u, 0u, 85u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 1u, 5u, 96u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 3u, 8u, 94u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 12u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 6u, 16u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 6u, 16u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 8u, 24u, 88u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 2u, 7u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 4u, 2u, 85u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 10u, 95u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 10u, 32u, 84u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 1u, 4u, 96u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 10u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 13u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 7u, 18u, 88u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 5u, 0u, 82u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_HEAL, TARGET_ALLY, 8u, 16u, 100u, SKILL_FLAG_FIELD_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 1u, 5u, 96u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 3u, 9u, 94u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 12u, 98u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 15u, 92u, SKILL_FLAG_IGNORE_DEF | SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 6u, 18u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ALL_ENEMY, 8u, 20u, 88u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 1u, 5u, 97u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 3u, 9u, 94u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 11u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 15u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 8u, 24u, 86u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 10u, 30u, 84u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 2u, 3u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 12u, 88u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 15u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 4u, 0u, 85u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DEBUFF, TARGET_ENEMY, 5u, 2u, 84u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 7u, 20u, 86u, SKILL_FLAG_IGNORE_DEF | SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 10u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 10u, 92u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 13u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 9u, 94u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 4u, 9u, 96u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 5u, 14u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 6u, 17u, 90u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ENEMY, 6u, 18u, 88u, SKILL_FLAG_AI_OK},
    {BATTLE_SKILL_KIND_HEAL, TARGET_ALLY, 7u, 18u, 100u, SKILL_FLAG_FIELD_OK},
    {BATTLE_SKILL_KIND_HEAL, TARGET_ALLY, 12u, 24u, 100u, SKILL_FLAG_FIELD_OK},
    {BATTLE_SKILL_KIND_HEAL, TARGET_ALLY, 6u, 8u, 100u, SKILL_FLAG_FIELD_OK},
    {BATTLE_SKILL_KIND_DAMAGE, TARGET_ALL_ENEMY, 14u, 36u, 80u, SKILL_FLAG_AI_OK}
};

static UINT8 meta_power(UINT8 skill_id) {
    if (skill_id >= SKILL_MAX) skill_id = SKILL_NONE;
    return skill_meta_table[skill_id].power;
}

UINT8 battle_skill_runtime_kind(UINT8 skill_id) BANKED {
    if (skill_id >= SKILL_MAX) skill_id = SKILL_NONE;
    return skill_meta_table[skill_id].kind;
}

UINT8 battle_skill_runtime_mp_cost(UINT8 skill_id) BANKED {
    if (skill_id >= SKILL_MAX) skill_id = SKILL_NONE;
    return skill_meta_table[skill_id].mp_cost;
}

UINT8 battle_skill_runtime_is_magic(UINT8 skill_id) BANKED {
    if (skill_id == SKILL_HEAL_SIMPLE || skill_id == SKILL_FIRE) return 1u;
    if (skill_id >= SKILL_MAGIC_FLAME && skill_id <= SKILL_MAGIC_METEOR) return 1u;
    if (skill_id >= SKILL_MAGIC_FREEZE && skill_id <= SKILL_MAGIC_GRAND_CROSS) return 1u;
    return 0u;
}

UINT8 battle_skill_runtime_is_heal_magic(UINT8 skill_id) BANKED {
    return (skill_id == SKILL_HEAL_SIMPLE || skill_id == SKILL_MAGIC_HEAL_PLUS ||
            skill_id == SKILL_MAGIC_REVIVE || skill_id == SKILL_MAGIC_BARRIER ||
            skill_id == SKILL_MAGIC_CURE_ALL) ? 1u : 0u;
}

static UINT16 meta_clamp_damage_i16(INT16 damage) {
    if (damage < 1) return 1u;
    if ((UINT16)damage > 999u) return 999u;
    return (UINT16)damage;
}

UINT16 battle_skill_runtime_calc_damage(UINT8 skill_id, UINT8 attack, UINT8 magic_power, UINT8 magic_mastery, UINT8 defense) BANKED {
    INT16 damage;
    UINT8 power;
    power = meta_power(skill_id);
    if (battle_skill_runtime_is_magic(skill_id)) {
        damage = (INT16)((UINT16)magic_power * 3u);
        damage += (INT16)(magic_mastery >> 2);
        damage += (INT16)power;
        damage -= (INT16)(defense >> 2);
    } else {
        damage = (INT16)((UINT16)attack * 2u);
        damage += (INT16)(power + (attack >> 1));
        damage -= (INT16)(defense >> 1);
    }
    return meta_clamp_damage_i16(damage);
}

UINT16 battle_skill_runtime_calc_heal(UINT8 skill_id, UINT8 magic_power, UINT8 magic_mastery) BANKED {
    return (UINT16)((UINT16)magic_power * 2u + (UINT16)(magic_mastery >> 2) + (UINT16)meta_power(skill_id));
}
