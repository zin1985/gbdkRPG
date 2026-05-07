#pragma bank 7

#include <gb/gb.h>
#include "party_runtime.h"

BANKREF(party_runtime_bank)

#define PARTY_WEAPON_NONE  0u
#define PARTY_WEAPON_SWORD 1u
#define PARTY_WEAPON_STAFF 2u
#define PARTY_WEAPON_BOW   3u
#define PARTY_WEAPON_COUNT 4u
#define PARTY_MASTERY_MAX  50u
#define PARTY_TECH_FLAG_SKILL 0x01u
#define PARTY_TECH_FLAG_HEAL  0x02u

typedef struct PartyMemberRuntime {
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
    UINT8 sprite_tile_offset;
    UINT8 weapon_type;
    UINT8 weapon_mastery[PARTY_WEAPON_COUNT];
    UINT8 learned_tech_flags;
} PartyMemberRuntime;

static PartyMemberRuntime party_roster[PARTY_ROSTER_COUNT];
static UINT8 party_active_slots[PARTY_ACTIVE_COUNT];

static void party_init_member(UINT8 id, const char *name, UINT16 hp, UINT16 mp,
                              UINT8 atk, UINT8 def, UINT8 skill, UINT8 heal,
                              UINT8 agi, UINT8 sprite_tile_offset) BANKED {
    if (id >= PARTY_ROSTER_COUNT) return;

    party_roster[id].name = name;
    party_roster[id].max_hp = hp;
    party_roster[id].hp = hp;
    party_roster[id].max_mp = mp;
    party_roster[id].mp = mp;
    party_roster[id].attack = atk;
    party_roster[id].defense = def;
    party_roster[id].skill_power = skill;
    party_roster[id].heal_power = heal;
    party_roster[id].agility = agi;
    party_roster[id].sprite_tile_offset = sprite_tile_offset;
    party_roster[id].weapon_type = PARTY_WEAPON_NONE;
    party_roster[id].weapon_mastery[0] = 0u;
    party_roster[id].weapon_mastery[1] = 0u;
    party_roster[id].weapon_mastery[2] = 0u;
    party_roster[id].weapon_mastery[3] = 0u;
    party_roster[id].learned_tech_flags = 0u;
}

void party_init_roster_defaults(void) BANKED {
    party_active_slots[0] = PARTY_MEMBER_HERO;
    party_active_slots[1] = PARTY_MEMBER_PRIEST;
    party_active_slots[2] = PARTY_MEMBER_MAGE;

    party_init_member(PARTY_MEMBER_HERO,    "ゆうしゃ", 38u, 24u, 7u, 3u, 8u, 10u, 8u, 0u);
    party_init_member(PARTY_MEMBER_PRIEST,  "そうりょ", 24u, 18u, 4u, 3u, 4u, 14u, 5u, 8u);
    party_init_member(PARTY_MEMBER_MAGE,    "まほう",   20u, 30u, 3u, 2u, 15u, 6u, 6u, 4u);

    party_init_member(PARTY_MEMBER_WARRIOR, "せんし",   42u,  6u, 9u, 5u, 3u, 3u, 4u, 0u);
    party_init_member(PARTY_MEMBER_ARCHER,  "かりうど", 30u, 12u, 7u, 3u, 6u, 5u, 9u, 4u);
    party_init_member(PARTY_MEMBER_MONK,    "ぶとうか", 34u, 10u, 8u, 4u, 5u, 5u, 10u, 8u);

    party_roster[PARTY_MEMBER_HERO].weapon_type = PARTY_WEAPON_SWORD;
    party_roster[PARTY_MEMBER_PRIEST].weapon_type = PARTY_WEAPON_STAFF;
    party_roster[PARTY_MEMBER_MAGE].weapon_type = PARTY_WEAPON_STAFF;
    party_roster[PARTY_MEMBER_WARRIOR].weapon_type = PARTY_WEAPON_SWORD;
    party_roster[PARTY_MEMBER_ARCHER].weapon_type = PARTY_WEAPON_BOW;
    party_roster[PARTY_MEMBER_MONK].weapon_type = PARTY_WEAPON_NONE;
}

void party_prepare_battle_members(UINT16 hero_max_hp, UINT16 hero_max_mp,
                                  UINT8 hero_attack, UINT8 hero_defense,
                                  UINT8 hero_skill_power, UINT8 hero_heal_power,
                                  UINT8 hero_agility) BANKED {
    PartyMemberRuntime *hero;

    hero = &party_roster[PARTY_MEMBER_HERO];
    hero->max_hp = hero_max_hp;
    hero->hp = hero_max_hp;
    hero->max_mp = hero_max_mp;
    hero->mp = hero_max_mp;
    hero->attack = hero_attack;
    hero->defense = hero_defense;
    hero->skill_power = hero_skill_power;
    hero->heal_power = hero_heal_power;
    hero->agility = hero_agility;
}

void party_sync_hero_from_values(UINT16 max_hp, UINT16 hp,
                                 UINT16 max_mp, UINT16 mp,
                                 UINT8 attack, UINT8 defense,
                                 UINT8 skill_power, UINT8 heal_power,
                                 UINT8 agility) BANKED {
    PartyMemberRuntime *hero;

    hero = &party_roster[PARTY_MEMBER_HERO];
    hero->max_hp = max_hp;
    hero->hp = hp;
    hero->max_mp = max_mp;
    hero->mp = mp;
    hero->attack = attack;
    hero->defense = defense;
    hero->skill_power = skill_power;
    hero->heal_power = heal_power;
    hero->agility = agility;
}

UINT8 party_get_active_member_id(UINT8 active_slot) BANKED {
    if (active_slot >= PARTY_ACTIVE_COUNT) return PARTY_MEMBER_HERO;
    return party_active_slots[active_slot];
}

static PartyMemberRuntime *party_get_active_member(UINT8 active_slot) BANKED {
    UINT8 member_id;

    if (active_slot >= PARTY_ACTIVE_COUNT) return 0;

    member_id = party_active_slots[active_slot];
    if (member_id >= PARTY_ROSTER_COUNT) return 0;

    return &party_roster[member_id];
}

const char *party_get_active_name(UINT8 active_slot) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0;

    return member->name;
}

UINT16 party_get_active_hp(UINT8 active_slot) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;

    return member->hp;
}

UINT16 party_get_active_mp(UINT8 active_slot) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;

    return member->mp;
}

void party_get_active_fighter(UINT8 active_slot, PartyBattleFighter *out) BANKED {
    PartyMemberRuntime *member;

    if (out == 0) return;

    member = party_get_active_member(active_slot);
    if (member == 0) {
        out->name = 0;
        out->max_hp = 0u;
        out->hp = 0u;
        out->max_mp = 0u;
        out->mp = 0u;
        out->attack = 0u;
        out->defense = 0u;
        out->skill_power = 0u;
        out->heal_power = 0u;
        out->agility = 0u;
        return;
    }

    out->name = member->name;
    out->max_hp = member->max_hp;
    out->hp = member->hp;
    out->max_mp = member->max_mp;
    out->mp = member->mp;
    out->attack = member->attack;
    out->defense = member->defense;
    out->skill_power = member->skill_power;
    out->heal_power = member->heal_power;
    out->agility = member->agility;
}

UINT8 party_active_alive_count(void) BANKED {
    UINT8 i;
    UINT8 count;
    PartyMemberRuntime *member;

    count = 0u;
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member != 0 && member->hp > 0u) count++;
    }
    return count;
}

UINT8 party_choose_random_alive_active_slot(UINT8 random_value) BANKED {
    UINT8 alive_count;
    UINT8 pick;
    UINT8 i;
    PartyMemberRuntime *member;

    alive_count = party_active_alive_count();
    if (alive_count == 0u) return 0u;

    pick = (UINT8)(random_value % alive_count);

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member != 0 && member->hp > 0u) {
            if (pick == 0u) return i;
            pick--;
        }
    }

    return 0u;
}

void party_damage_active(UINT8 active_slot, UINT16 damage) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return;

    if (damage >= member->hp) member->hp = 0u;
    else member->hp = (UINT16)(member->hp - damage);
}


static void party_gain_mastery(PartyMemberRuntime *member, UINT8 tech_flag) BANKED {
    UINT8 weapon_type;

    if (member == 0) return;

    weapon_type = member->weapon_type;
    if (weapon_type < PARTY_WEAPON_COUNT && member->weapon_mastery[weapon_type] < PARTY_MASTERY_MAX) {
        member->weapon_mastery[weapon_type]++;
    }
    member->learned_tech_flags |= tech_flag;
}

UINT16 party_battle_op(UINT8 op, UINT8 active_slot, UINT16 value) BANKED {
    UINT16 after;
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;

    if (op == PARTY_OP_TRY_CONSUME_MP) {
        if (member->mp < value) return 0u;
        member->mp = (UINT16)(member->mp - value);
        party_gain_mastery(member, PARTY_TECH_FLAG_SKILL);
        return 1u;
    }

    if (op == PARTY_OP_HEAL_ACTIVE) {
        if (member->hp == 0u) return member->hp;

        after = (UINT16)(member->hp + value);
        if (after > member->max_hp || after < member->hp) {
            after = member->max_hp;
        }
        member->hp = after;
        party_gain_mastery(member, PARTY_TECH_FLAG_HEAL);
        return member->hp;
    }

    return 0u;
}

UINT8 party_swap_active_with_reserve(UINT8 active_slot, UINT8 reserve_member_id) BANKED {
    UINT8 i;

    if (active_slot >= PARTY_ACTIVE_COUNT) return 0u;
    if (reserve_member_id >= PARTY_ROSTER_COUNT) return 0u;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        if (party_active_slots[i] == reserve_member_id) return 0u;
    }

    party_active_slots[active_slot] = reserve_member_id;
    return 1u;
}
