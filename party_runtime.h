#ifndef PARTY_RUNTIME_H
#define PARTY_RUNTIME_H

#include <gb/gb.h>
#include "skill_defs.h"

#define PARTY_ACTIVE_COUNT 3u
#define PARTY_ROSTER_COUNT 6u

#define PARTY_MEMBER_HERO    0u
#define PARTY_MEMBER_PRIEST  1u
#define PARTY_MEMBER_MAGE    2u
#define PARTY_MEMBER_WARRIOR 3u
#define PARTY_MEMBER_ARCHER  4u
#define PARTY_MEMBER_MONK    5u

#define PARTY_OP_TRY_CONSUME_MP 2u
#define PARTY_OP_HEAL_ACTIVE    3u
#define PARTY_OP_NOTE_ATTACK     4u
#define PARTY_OP_NOTE_MAGIC      5u


#define PARTY_WEAPON_NONE  0u
#define PARTY_WEAPON_SWORD 1u
#define PARTY_WEAPON_STAFF 2u
#define PARTY_WEAPON_BOW   3u
#define PARTY_WEAPON_GLOVE 4u
#define PARTY_WEAPON_TOOL  5u
#define PARTY_WEAPON_SPEAR 6u
#define PARTY_WEAPON_AXE   7u
#define PARTY_WEAPON_COUNT 8u

typedef struct PartySaveMember {
    UINT16 max_hp;
    UINT16 hp;
    UINT16 max_mp;
    UINT16 mp;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill_power;
    UINT8 heal_power;
    UINT8 agility;
    UINT8 weapon_id;
    UINT8 armor_id;
    UINT8 accessory_id;
    UINT8 weapon_mastery[PARTY_WEAPON_COUNT];
    UINT8 magic_mastery;
    UINT8 learned_tech_flags;
    UINT8 learned_magic_flags;
    UINT8 learned_skills[PLAYER_SKILL_SLOT_COUNT];
    UINT8 morale;
    UINT8 focus;
    UINT8 adventure_sense;
    UINT8 fatigue;
} PartySaveMember;

typedef struct PartySaveState {
    PartySaveMember active[PARTY_ACTIVE_COUNT];
} PartySaveState;

typedef struct PartyBattleFighter {
    const char *name;
    UINT16 max_hp;
    UINT16 hp;
    UINT16 max_mp;
    UINT16 mp;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill_power;
    UINT8 heal_power;
    UINT8 magic_mastery;
    UINT8 agility;
} PartyBattleFighter;

BANKREF_EXTERN(party_runtime_bank)

void party_init_roster_defaults(void) BANKED;
void party_prepare_battle_members(UINT16 hero_max_hp, UINT16 hero_max_mp,
                                  UINT8 hero_attack, UINT8 hero_defense,
                                  UINT8 hero_skill_power, UINT8 hero_heal_power,
                                  UINT8 hero_agility) BANKED;
void party_sync_hero_from_values(UINT16 max_hp, UINT16 hp,
                                 UINT16 max_mp, UINT16 mp,
                                 UINT8 attack, UINT8 defense,
                                 UINT8 skill_power, UINT8 heal_power,
                                 UINT8 agility) BANKED;

UINT8 party_get_active_member_id(UINT8 active_slot) BANKED;
const char *party_get_active_name(UINT8 active_slot) BANKED;
void party_put_active_name_battle(UINT8 active_slot, UINT8 x, UINT8 y) BANKED;
UINT16 party_get_active_hp(UINT8 active_slot) BANKED;
UINT16 party_get_active_mp(UINT8 active_slot) BANKED;
void party_get_active_fighter(UINT8 active_slot, PartyBattleFighter *out) BANKED;

UINT8 party_active_alive_count(void) BANKED;
UINT8 party_choose_random_alive_active_slot(UINT8 random_value) BANKED;
void party_damage_active(UINT8 active_slot, UINT16 damage) BANKED;
UINT16 party_battle_op(UINT8 op, UINT8 active_slot, UINT16 value) BANKED;
UINT16 party_heal_active(UINT8 active_slot, UINT16 amount) BANKED;
void party_heal_all_active(void) BANKED;
void party_menu_show_heal_skill_loop(void) BANKED;
void party_after_battle_growth(UINT8 enemy_rank, UINT8 random_seed) BANKED;
UINT8 party_try_spark_skill(UINT8 active_slot, UINT8 random_seed, UINT8 *skill_id_out) BANKED;
UINT8 party_try_spark_magic(UINT8 active_slot, UINT8 random_seed, UINT8 *magic_id_out) BANKED;
UINT8 party_try_spark_magic_from(UINT8 active_slot, UINT8 trigger_skill_id, UINT8 random_seed, UINT8 *magic_id_out) BANKED;

UINT8 party_get_learned_skill_count(UINT8 active_slot, UINT8 magic_mode) BANKED;
UINT8 party_get_learned_skill_at(UINT8 active_slot, UINT8 index, UINT8 magic_mode) BANKED;
UINT8 party_has_learned_skill(UINT8 active_slot, UINT8 skill_id) BANKED;
UINT8 party_add_learned_skill(UINT8 active_slot, UINT8 skill_id) BANKED;

UINT8 party_swap_active_with_reserve(UINT8 active_slot, UINT8 reserve_member_id) BANKED;
void party_menu_show_status_loop(void) BANKED;
void party_menu_show_equip_loop(void) BANKED;
UINT8 party_use_field_item_on_active(UINT8 item_id, UINT8 active_slot) BANKED;
UINT8 party_get_last_item_effect_amount(void) BANKED;
UINT8 party_get_last_item_effect_kind(void) BANKED;
UINT8 party_debug_no_encounter_accessory_equipped(void) BANKED;
UINT8 party_debug_escape_accessory_equipped(void) BANKED;
void party_save_copy_to(PartySaveState *dst) BANKED;
void party_save_copy_from(const PartySaveState *src) BANKED;

#endif
