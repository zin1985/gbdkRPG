#ifndef BATTLE_SKILL_RUNTIME_H
#define BATTLE_SKILL_RUNTIME_H

#include <gb/gb.h>

#define BATTLE_SKILL_EVENT_NONE        0u
#define BATTLE_SKILL_EVENT_CANCEL      1u
#define BATTLE_SKILL_EVENT_ENEMY_SKILL 2u
#define BATTLE_SKILL_EVENT_ALLY_SKILL  3u

#define BATTLE_SKILL_KIND_DAMAGE 0u
#define BATTLE_SKILL_KIND_HEAL   1u
#define BATTLE_SKILL_KIND_DEBUFF 2u

void battle_skill_runtime_start(UINT8 party_turn_slot) BANKED;
void battle_skill_runtime_start_magic(UINT8 party_turn_slot) BANKED;
UINT8 battle_skill_runtime_update(UINT8 *skill_id) BANKED;
void battle_skill_runtime_start_ally(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_update_ally(UINT8 *skill_id, UINT8 *ally_slot) BANKED;

UINT8 battle_skill_runtime_kind(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_mp_cost(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_is_magic(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_is_heal_magic(UINT8 skill_id) BANKED;
UINT16 battle_skill_runtime_calc_damage(UINT8 skill_id, UINT8 attack, UINT8 magic_power, UINT8 magic_mastery, UINT8 defense) BANKED;
UINT16 battle_skill_runtime_calc_heal(UINT8 skill_id, UINT8 magic_power, UINT8 magic_mastery) BANKED;
const char *battle_skill_runtime_name(UINT8 skill_id) BANKED;
const char *battle_skill_runtime_name_buffered(UINT8 skill_id) BANKED;
void battle_skill_runtime_put_name(UINT8 x, UINT8 y, UINT8 skill_id) BANKED;

/* rpg235: spark-derivation API. Lookup tables live in bank 20. */
UINT8 battle_skill_runtime_magic_category(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_is_buff_magic(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_is_all_target(UINT8 skill_id) BANKED;
UINT8 battle_skill_spark_pick_tech(UINT8 weapon_type, UINT8 mastery, UINT8 seed, const UINT8 *learned_skills) BANKED;
UINT8 battle_skill_spark_pick_tech_starter(UINT8 weapon_type, UINT8 mastery, UINT8 seed, const UINT8 *learned_skills) BANKED;
UINT8 battle_skill_spark_pick_tech_from(UINT8 weapon_type, UINT8 trigger_skill_id, UINT8 mastery, UINT8 seed, const UINT8 *learned_skills) BANKED;
UINT8 battle_skill_spark_pick_magic(UINT8 magic_category, UINT8 mastery, UINT8 seed, const UINT8 *learned_skills) BANKED;

#endif
