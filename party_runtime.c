#pragma bank 7

#include <gb/gb.h>
#include "party_runtime.h"

BANKREF(party_runtime_bank)

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
