#pragma bank 7

#include <gb/gb.h>
#include "party_runtime.h"

BANKREF(party_runtime_bank)

#define PARTY_WEAPON_NONE  0u
#define PARTY_WEAPON_SWORD 1u
#define PARTY_WEAPON_STAFF 2u
#define PARTY_WEAPON_BOW   3u
#define PARTY_WEAPON_GLOVE 4u
#define PARTY_WEAPON_TOOL  5u
#define PARTY_WEAPON_COUNT 6u
#define PARTY_MASTERY_MAX  50u

/* rpg115: original growth pack without LP.
 * Keep all new gameplay foundations inside bank 7 and do not add public APIs.
 * main.c and party_runtime.h are intentionally unchanged to protect Bank 0.
 */
#define PARTY_TECH_FLAG_SKILL   0x01u
#define PARTY_TECH_FLAG_HEAL    0x02u
#define PARTY_TECH_FLAG_RANK1   0x04u
#define PARTY_TECH_FLAG_RANK2   0x08u
#define PARTY_TECH_FLAG_SPARK   0x10u
#define PARTY_TECH_FLAG_FOCUS   0x20u
#define PARTY_TECH_FLAG_COUNTER 0x40u
#define PARTY_TECH_FLAG_TOOL    0x80u

#define PARTY_MAGIC_FLAG_FIRE    0x01u
#define PARTY_MAGIC_FLAG_ICE     0x02u
#define PARTY_MAGIC_FLAG_HEAL    0x04u
#define PARTY_MAGIC_FLAG_LIGHT   0x08u
#define PARTY_MAGIC_FLAG_THUNDER 0x10u
#define PARTY_MAGIC_FLAG_DARK    0x20u

#define PARTY_RACE_HUMAN   0u
#define PARTY_RACE_MUTANT  1u
#define PARTY_RACE_MECH    2u
#define PARTY_RACE_MONSTER 3u

#define PARTY_STATUS_POISON 0x01u
#define PARTY_STATUS_SLEEP  0x02u
#define PARTY_STATUS_STUN   0x04u
#define PARTY_STATUS_TIRED  0x08u
#define PARTY_STATUS_DOWN   0x80u

#define PARTY_ELEM_FIRE    0x01u
#define PARTY_ELEM_ICE     0x02u
#define PARTY_ELEM_THUNDER 0x04u
#define PARTY_ELEM_LIGHT   0x08u
#define PARTY_ELEM_DARK    0x10u

#define PARTY_FORM_FRONT 0u
#define PARTY_FORM_BACK  1u
#define PARTY_FORM_SWIFT 2u
#define PARTY_FORM_GUARD 3u
#define PARTY_FORM_FOCUS 4u

#define PARTY_ACTION_NONE   0u
#define PARTY_ACTION_SKILL  1u
#define PARTY_ACTION_HEAL   2u
#define PARTY_ACTION_DAMAGE 3u

#define PARTY_MORALE_MAX  31u
#define PARTY_FOCUS_MAX   31u
#define PARTY_SENSE_MAX   63u
#define PARTY_FATIGUE_MAX 31u

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
    UINT8 learned_magic_flags;
    UINT8 race_type;
    UINT8 equip_weight;
    UINT8 formation_role;
    UINT8 status_flags;
    UINT8 element_resist_flags;
    UINT8 element_weak_flags;
    UINT8 morale;
    UINT8 focus;
    UINT8 adventure_sense;
    UINT8 fatigue;
    UINT8 last_action_style;
} PartyMemberRuntime;

static PartyMemberRuntime party_roster[PARTY_ROSTER_COUNT];
static UINT8 party_active_slots[PARTY_ACTIVE_COUNT];

static void party_init_member(UINT8 id, const char *name, UINT16 hp, UINT16 mp,
                              UINT8 atk, UINT8 def, UINT8 skill, UINT8 heal,
                              UINT8 agi, UINT8 sprite_tile_offset) BANKED {
    UINT8 i;
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
    for (i = 0u; i < PARTY_WEAPON_COUNT; i++) party_roster[id].weapon_mastery[i] = 0u;
    party_roster[id].learned_tech_flags = 0u;
    party_roster[id].learned_magic_flags = 0u;
    party_roster[id].race_type = PARTY_RACE_HUMAN;
    party_roster[id].equip_weight = 2u;
    party_roster[id].formation_role = PARTY_FORM_FRONT;
    party_roster[id].status_flags = 0u;
    party_roster[id].element_resist_flags = 0u;
    party_roster[id].element_weak_flags = 0u;
    party_roster[id].morale = 2u;
    party_roster[id].focus = 0u;
    party_roster[id].adventure_sense = 0u;
    party_roster[id].fatigue = 0u;
    party_roster[id].last_action_style = PARTY_ACTION_NONE;
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
    party_roster[PARTY_MEMBER_MONK].weapon_type = PARTY_WEAPON_GLOVE;

    /* Original-growth defaults. No LP: risk is expressed through morale,
     * focus, adventure sense, fatigue, formation, and derived stats.
     */
    party_roster[PARTY_MEMBER_HERO].adventure_sense = 4u;
    party_roster[PARTY_MEMBER_HERO].morale = 5u;

    party_roster[PARTY_MEMBER_PRIEST].formation_role = PARTY_FORM_BACK;
    party_roster[PARTY_MEMBER_PRIEST].learned_magic_flags = PARTY_MAGIC_FLAG_HEAL | PARTY_MAGIC_FLAG_LIGHT;
    party_roster[PARTY_MEMBER_PRIEST].element_resist_flags = PARTY_ELEM_LIGHT;
    party_roster[PARTY_MEMBER_PRIEST].focus = 2u;

    party_roster[PARTY_MEMBER_MAGE].race_type = PARTY_RACE_MUTANT;
    party_roster[PARTY_MEMBER_MAGE].formation_role = PARTY_FORM_BACK;
    party_roster[PARTY_MEMBER_MAGE].learned_magic_flags = PARTY_MAGIC_FLAG_FIRE | PARTY_MAGIC_FLAG_ICE | PARTY_MAGIC_FLAG_THUNDER;
    party_roster[PARTY_MEMBER_MAGE].element_resist_flags = PARTY_ELEM_FIRE | PARTY_ELEM_ICE;
    party_roster[PARTY_MEMBER_MAGE].element_weak_flags = PARTY_ELEM_DARK;
    party_roster[PARTY_MEMBER_MAGE].focus = 3u;

    party_roster[PARTY_MEMBER_WARRIOR].formation_role = PARTY_FORM_GUARD;
    party_roster[PARTY_MEMBER_WARRIOR].equip_weight = 5u;
    party_roster[PARTY_MEMBER_WARRIOR].morale = 7u;

    party_roster[PARTY_MEMBER_ARCHER].formation_role = PARTY_FORM_SWIFT;
    party_roster[PARTY_MEMBER_ARCHER].equip_weight = 1u;
    party_roster[PARTY_MEMBER_ARCHER].adventure_sense = 6u;

    party_roster[PARTY_MEMBER_MONK].formation_role = PARTY_FORM_FOCUS;
    party_roster[PARTY_MEMBER_MONK].equip_weight = 0u;
    party_roster[PARTY_MEMBER_MONK].morale = 6u;
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
    hero->status_flags = 0u;
    if (hero->morale < 3u) hero->morale = 3u;
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

static UINT8 party_clamp_u8(UINT16 value) BANKED {
    if (value > 255u) return 255u;
    return (UINT8)value;
}

static void party_add_capped(UINT8 *value, UINT8 add, UINT8 max) BANKED {
    UINT16 v;

    if (value == 0) return;
    v = (UINT16)(*value + add);
    if (v > max) *value = max;
    else *value = (UINT8)v;
}

static void party_decay_one(UINT8 *value) BANKED {
    if (value != 0 && *value > 0u) (*value)--;
}

static UINT8 party_mastery_bonus(PartyMemberRuntime *member) BANKED {
    UINT8 weapon_type;

    if (member == 0) return 0u;
    weapon_type = member->weapon_type;
    if (weapon_type >= PARTY_WEAPON_COUNT) return 0u;

    return (UINT8)(member->weapon_mastery[weapon_type] >> 3);
}

static UINT8 party_morale_bonus(PartyMemberRuntime *member) BANKED {
    if (member == 0) return 0u;
    return (UINT8)(member->morale >> 4);
}

static UINT8 party_focus_bonus(PartyMemberRuntime *member) BANKED {
    if (member == 0) return 0u;
    return (UINT8)(member->focus >> 3);
}

static UINT8 party_sense_bonus(PartyMemberRuntime *member) BANKED {
    if (member == 0) return 0u;
    return (UINT8)(member->adventure_sense >> 4);
}

static UINT8 party_fatigue_penalty(PartyMemberRuntime *member) BANKED {
    if (member == 0) return 0u;
    return (UINT8)(member->fatigue >> 3);
}

static void party_note_action(PartyMemberRuntime *member, UINT8 style) BANKED {
    if (member == 0) return;

    if (member->last_action_style == style) {
        party_add_capped(&member->focus, 2u, PARTY_FOCUS_MAX);
    } else {
        party_add_capped(&member->focus, 1u, PARTY_FOCUS_MAX);
    }
    member->last_action_style = style;

    if (style == PARTY_ACTION_DAMAGE) {
        party_add_capped(&member->morale, 3u, PARTY_MORALE_MAX);
        party_add_capped(&member->adventure_sense, 1u, PARTY_SENSE_MAX);
        party_add_capped(&member->fatigue, 2u, PARTY_FATIGUE_MAX);
    } else {
        party_add_capped(&member->adventure_sense, 1u, PARTY_SENSE_MAX);
        party_decay_one(&member->fatigue);
    }
}

static UINT8 party_effective_attack(PartyMemberRuntime *member) BANKED {
    UINT16 value;

    if (member == 0) return 0u;
    value = member->attack;
    value += party_mastery_bonus(member);
    value += party_morale_bonus(member);
    if (member->learned_tech_flags & PARTY_TECH_FLAG_RANK1) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_RANK2) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_COUNTER) value++;
    if (member->formation_role == PARTY_FORM_BACK && member->weapon_type != PARTY_WEAPON_BOW) {
        if (value > 1u) value--;
    }
    if (member->status_flags & PARTY_STATUS_POISON) {
        if (value > 0u) value--;
    }
    return party_clamp_u8(value);
}

static UINT8 party_effective_defense(PartyMemberRuntime *member) BANKED {
    UINT16 value;
    UINT8 penalty;

    if (member == 0) return 0u;
    value = member->defense;
    penalty = party_fatigue_penalty(member);
    if (member->formation_role == PARTY_FORM_GUARD) value += 2u;
    if (member->race_type == PARTY_RACE_MECH) value += 2u;
    if (member->adventure_sense >= 32u) value++;
    if (member->status_flags & PARTY_STATUS_POISON) {
        if (value > 0u) value--;
    }
    if (value > penalty) value -= penalty;
    else value = 1u;
    return party_clamp_u8(value);
}

static UINT8 party_effective_agility(PartyMemberRuntime *member) BANKED {
    UINT16 value;
    UINT8 penalty;

    if (member == 0) return 0u;
    value = member->agility;
    if (member->formation_role == PARTY_FORM_SWIFT) value += 2u;
    if (member->formation_role == PARTY_FORM_FOCUS && member->focus >= 16u) value++;
    value += party_sense_bonus(member);
    penalty = (UINT8)((member->equip_weight >> 1) + party_fatigue_penalty(member));
    if (value > penalty) value -= penalty;
    else value = 1u;
    if (member->status_flags & PARTY_STATUS_SLEEP) value = 1u;
    return party_clamp_u8(value);
}

static UINT8 party_effective_skill(PartyMemberRuntime *member) BANKED {
    UINT16 value;

    if (member == 0) return 0u;
    value = member->skill_power;
    value += party_mastery_bonus(member);
    value += party_focus_bonus(member);
    if (member->race_type == PARTY_RACE_MUTANT) value++;
    if (member->learned_magic_flags & (PARTY_MAGIC_FLAG_FIRE | PARTY_MAGIC_FLAG_ICE | PARTY_MAGIC_FLAG_THUNDER)) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_SPARK) value++;
    if (member->weapon_type == PARTY_WEAPON_TOOL && (member->learned_tech_flags & PARTY_TECH_FLAG_TOOL)) value++;
    return party_clamp_u8(value);
}

static UINT8 party_effective_heal(PartyMemberRuntime *member) BANKED {
    UINT16 value;

    if (member == 0) return 0u;
    value = member->heal_power;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_HEAL) value++;
    if (member->learned_magic_flags & (PARTY_MAGIC_FLAG_HEAL | PARTY_MAGIC_FLAG_LIGHT)) value++;
    if (member->race_type == PARTY_RACE_MUTANT) value++;
    value += party_focus_bonus(member);
    if (member->adventure_sense >= 24u) value++;
    return party_clamp_u8(value);
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
    out->attack = party_effective_attack(member);
    out->defense = party_effective_defense(member);
    out->skill_power = party_effective_skill(member);
    out->heal_power = party_effective_heal(member);
    out->agility = party_effective_agility(member);
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
    UINT16 low_hp;

    member = party_get_active_member(active_slot);
    if (member == 0) return;

    if (damage >= member->hp) {
        member->hp = 0u;
        member->status_flags |= PARTY_STATUS_DOWN;
        party_add_capped(&member->morale, 6u, PARTY_MORALE_MAX);
        party_add_capped(&member->fatigue, 3u, PARTY_FATIGUE_MAX);
        party_note_action(member, PARTY_ACTION_DAMAGE);
        return;
    }

    member->hp = (UINT16)(member->hp - damage);
    low_hp = (UINT16)(member->max_hp >> 2);
    if (low_hp == 0u) low_hp = 1u;
    if (member->hp <= low_hp) {
        party_add_capped(&member->morale, 4u, PARTY_MORALE_MAX);
        party_add_capped(&member->focus, 1u, PARTY_FOCUS_MAX);
    }
    if (damage > (member->max_hp >> 3)) party_add_capped(&member->fatigue, 1u, PARTY_FATIGUE_MAX);
    party_note_action(member, PARTY_ACTION_DAMAGE);
}

static void party_gain_mastery(PartyMemberRuntime *member, UINT8 tech_flag) BANKED {
    UINT8 weapon_type;

    if (member == 0) return;

    weapon_type = member->weapon_type;
    if (weapon_type < PARTY_WEAPON_COUNT && member->weapon_mastery[weapon_type] < PARTY_MASTERY_MAX) {
        member->weapon_mastery[weapon_type]++;
        if (member->weapon_mastery[weapon_type] >= 6u) member->learned_tech_flags |= PARTY_TECH_FLAG_RANK1;
        if (member->weapon_mastery[weapon_type] >= 16u) member->learned_tech_flags |= PARTY_TECH_FLAG_RANK2;
        if (member->weapon_mastery[weapon_type] >= 28u) member->learned_tech_flags |= PARTY_TECH_FLAG_SPARK;
        if (weapon_type == PARTY_WEAPON_STAFF && member->weapon_mastery[weapon_type] >= 10u) {
            member->learned_magic_flags |= PARTY_MAGIC_FLAG_HEAL;
        }
        if (weapon_type == PARTY_WEAPON_BOW && member->weapon_mastery[weapon_type] >= 12u) {
            member->learned_magic_flags |= PARTY_MAGIC_FLAG_THUNDER;
        }
        if (weapon_type == PARTY_WEAPON_GLOVE && member->weapon_mastery[weapon_type] >= 14u) {
            member->learned_tech_flags |= PARTY_TECH_FLAG_COUNTER;
        }
        if (weapon_type == PARTY_WEAPON_TOOL && member->weapon_mastery[weapon_type] >= 8u) {
            member->learned_tech_flags |= PARTY_TECH_FLAG_TOOL;
        }
    }
    member->learned_tech_flags |= tech_flag;
}

UINT16 party_battle_op(UINT8 op, UINT8 active_slot, UINT16 value) BANKED {
    UINT16 after;
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;

    if (op == PARTY_OP_TRY_CONSUME_MP) {
        if ((member->learned_tech_flags & PARTY_TECH_FLAG_RANK1) && value > 0u) value--;
        if ((member->focus >= 20u) && value > 0u) value--;
        if (member->mp < value) return 0u;
        member->mp = (UINT16)(member->mp - value);
        party_gain_mastery(member, PARTY_TECH_FLAG_SKILL);
        if (member->focus >= 16u) member->learned_tech_flags |= PARTY_TECH_FLAG_FOCUS;
        party_note_action(member, PARTY_ACTION_SKILL);
        return 1u;
    }

    if (op == PARTY_OP_HEAL_ACTIVE) {
        if (member->hp == 0u) return member->hp;

        value = (UINT16)(value + (party_mastery_bonus(member) >> 1));
        value = (UINT16)(value + party_focus_bonus(member));
        if (member->race_type == PARTY_RACE_MUTANT) value++;
        if (member->adventure_sense >= 32u) value++;
        after = (UINT16)(member->hp + value);
        if (after > member->max_hp || after < member->hp) {
            after = member->max_hp;
        }
        member->hp = after;
        member->status_flags &= (UINT8)~(PARTY_STATUS_SLEEP | PARTY_STATUS_STUN | PARTY_STATUS_DOWN | PARTY_STATUS_TIRED);
        if (member->fatigue > 1u) member->fatigue = (UINT8)(member->fatigue - 2u);
        else member->fatigue = 0u;
        party_gain_mastery(member, PARTY_TECH_FLAG_HEAL);
        party_note_action(member, PARTY_ACTION_HEAL);
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
