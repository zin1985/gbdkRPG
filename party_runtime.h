#ifndef PARTY_RUNTIME_H
#define PARTY_RUNTIME_H

#include <gb/gb.h>

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
UINT16 party_get_active_hp(UINT8 active_slot) BANKED;
UINT16 party_get_active_mp(UINT8 active_slot) BANKED;
void party_get_active_fighter(UINT8 active_slot, PartyBattleFighter *out) BANKED;

UINT8 party_active_alive_count(void) BANKED;
UINT8 party_choose_random_alive_active_slot(UINT8 random_value) BANKED;
void party_damage_active(UINT8 active_slot, UINT16 damage) BANKED;
UINT16 party_battle_op(UINT8 op, UINT8 active_slot, UINT16 value) BANKED;

UINT8 party_swap_active_with_reserve(UINT8 active_slot, UINT8 reserve_member_id) BANKED;
void party_menu_show_status_loop(void) BANKED;
void party_menu_show_equip_loop(void) BANKED;

#endif
