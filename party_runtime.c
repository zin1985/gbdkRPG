#pragma bank 7

#include <gb/gb.h>
#include "party_runtime.h"
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"

BANKREF(party_runtime_bank)

#ifndef PARTY_TECH_FLAG_NONE
#define PARTY_TECH_FLAG_NONE 0x00u
#endif
#ifndef PARTY_ACTION_ATTACK
#define PARTY_ACTION_ATTACK 4u
#endif

#ifndef PARTY_WEAPON_COUNT
#define PARTY_WEAPON_NONE  0u
#define PARTY_WEAPON_SWORD 1u
#define PARTY_WEAPON_STAFF 2u
#define PARTY_WEAPON_BOW   3u
#define PARTY_WEAPON_GLOVE 4u
#define PARTY_WEAPON_TOOL  5u
#define PARTY_WEAPON_COUNT 6u
#endif
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

#define PARTY_EQUIP_SLOT_WEAPON 0u
#define PARTY_EQUIP_SLOT_ARMOR  1u
#define PARTY_EQUIP_SLOT_ACC    2u

/* rpg116: equipment seed. Kept in bank 7, not in Bank 0.
 * Item IDs are defined in inventory.h so inventory, equipment and menu pages
 * share the same small numeric IDs.
 */
typedef struct PartyEquipmentDef {
    UINT8 item_id;
    UINT8 slot;
    UINT8 weapon_type;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill;
    UINT8 heal;
    UINT8 agility;
    UINT8 weight;
    UINT8 element_resist;
    UINT8 status_resist;
    const char *name;
} PartyEquipmentDef;

static const PartyEquipmentDef party_equipment_defs[] = {
    { ITEM_WOOD_SWORD,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 2u, 0u, 0u, 0u, 0u, 1u, 0u, 0u, "木のけん" },
    { ITEM_IRON_SWORD,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 4u, 0u, 0u, 0u, 0u, 3u, 0u, 0u, "鉄のけん" },
    { ITEM_OAK_STAFF,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 1u, 0u, 0u, 2u, 0u, 1u, PARTY_ELEM_LIGHT, 0u, "木のつえ" },
    { ITEM_MAGE_STAFF,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 1u, 0u, 3u, 1u, 0u, 1u, PARTY_ELEM_FIRE | PARTY_ELEM_ICE, 0u, "まどうのつえ" },
    { ITEM_SHORT_BOW,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   3u, 0u, 0u, 0u, 1u, 1u, PARTY_ELEM_THUNDER, 0u, "短いゆみ" },
    { ITEM_GLOVES,        PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 2u, 0u, 1u, 0u, 2u, 0u, 0u, 0u, "けいこ手甲" },
    { ITEM_TOOL_KIT,      PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  1u, 0u, 2u, 0u, 0u, 1u, 0u, 0u, "道具キット" },
    { ITEM_CLOTH_ARMOR,   PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 1u, 0u, 0u, 0u, 1u, 0u, 0u, "ぬののふく" },
    { ITEM_LEATHER_ARMOR, PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 2u, 0u, 0u, 0u, 2u, 0u, 0u, "かわよろい" },
    { ITEM_IRON_ARMOR,    PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 4u, 0u, 0u, 0u, 5u, 0u, 0u, "鉄よろい" },
    { ITEM_CHARM,         PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 1u, 1u, 0u, 0u, PARTY_ELEM_LIGHT, PARTY_STATUS_SLEEP, "おまもり" },
    { ITEM_FEATHER,       PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 2u, 0u, 0u, 0u, "はね飾り" },
    { ITEM_DEBUG_NO_ENCOUNT, PARTY_EQUIP_SLOT_ACC,  PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, "退魔のすず" },
    { ITEM_DEBUG_ESCAPE, PARTY_EQUIP_SLOT_ACC,  PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, "にげあしリング" }
};
#define PARTY_EQUIPMENT_DEF_COUNT ((UINT8)(sizeof(party_equipment_defs) / sizeof(party_equipment_defs[0])))


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
    UINT8 weapon_id;
    UINT8 armor_id;
    UINT8 accessory_id;
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
    party_roster[id].weapon_id = ITEM_NONE;
    party_roster[id].armor_id = ITEM_NONE;
    party_roster[id].accessory_id = ITEM_NONE;
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

    party_roster[PARTY_MEMBER_HERO].weapon_id = ITEM_IRON_SWORD;
    party_roster[PARTY_MEMBER_HERO].armor_id = ITEM_LEATHER_ARMOR;
    party_roster[PARTY_MEMBER_HERO].accessory_id = ITEM_DEBUG_ESCAPE;
    party_roster[PARTY_MEMBER_HERO].weapon_type = PARTY_WEAPON_SWORD;

    party_roster[PARTY_MEMBER_PRIEST].weapon_id = ITEM_OAK_STAFF;
    party_roster[PARTY_MEMBER_PRIEST].armor_id = ITEM_CLOTH_ARMOR;
    party_roster[PARTY_MEMBER_PRIEST].accessory_id = ITEM_CHARM;
    party_roster[PARTY_MEMBER_PRIEST].weapon_type = PARTY_WEAPON_STAFF;

    party_roster[PARTY_MEMBER_MAGE].weapon_id = ITEM_MAGE_STAFF;
    party_roster[PARTY_MEMBER_MAGE].armor_id = ITEM_CLOTH_ARMOR;
    party_roster[PARTY_MEMBER_MAGE].accessory_id = ITEM_CHARM;
    party_roster[PARTY_MEMBER_MAGE].weapon_type = PARTY_WEAPON_STAFF;

    party_roster[PARTY_MEMBER_WARRIOR].weapon_id = ITEM_IRON_SWORD;
    party_roster[PARTY_MEMBER_WARRIOR].armor_id = ITEM_IRON_ARMOR;
    party_roster[PARTY_MEMBER_WARRIOR].accessory_id = ITEM_CHARM;
    party_roster[PARTY_MEMBER_WARRIOR].weapon_type = PARTY_WEAPON_SWORD;

    party_roster[PARTY_MEMBER_ARCHER].weapon_id = ITEM_SHORT_BOW;
    party_roster[PARTY_MEMBER_ARCHER].armor_id = ITEM_LEATHER_ARMOR;
    party_roster[PARTY_MEMBER_ARCHER].accessory_id = ITEM_FEATHER;
    party_roster[PARTY_MEMBER_ARCHER].weapon_type = PARTY_WEAPON_BOW;

    party_roster[PARTY_MEMBER_MONK].weapon_id = ITEM_GLOVES;
    party_roster[PARTY_MEMBER_MONK].armor_id = ITEM_CLOTH_ARMOR;
    party_roster[PARTY_MEMBER_MONK].accessory_id = ITEM_FEATHER;
    party_roster[PARTY_MEMBER_MONK].weapon_type = PARTY_WEAPON_GLOVE;

    inventory_seed_defaults();

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

void party_put_active_name_battle(UINT8 active_slot, UINT8 x, UINT8 y) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return;

    /* rpg160: member->name points to string literals owned by this bank.
     * Draw the name while this bank is active; returning the pointer to
     * Bank 0 and drawing later can show blank/garbled text.
     */
    jp_put_bkg_text(x, y, member->name);
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



UINT8 party_debug_no_encounter_accessory_equipped(void) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member != 0 && member->accessory_id == ITEM_DEBUG_NO_ENCOUNT) {
            return 1u;
        }
    }
    return 0u;
}

UINT8 party_debug_escape_accessory_equipped(void) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member != 0 && member->accessory_id == ITEM_DEBUG_ESCAPE) {
            return 1u;
        }
    }
    return 0u;
}

static const PartyEquipmentDef *party_find_equipment(UINT8 item_id) BANKED {
    UINT8 i;

    if (item_id == ITEM_NONE) return 0;
    for (i = 0u; i < PARTY_EQUIPMENT_DEF_COUNT; i++) {
        if (party_equipment_defs[i].item_id == item_id) return &party_equipment_defs[i];
    }
    return 0;
}

static const char *party_equipment_name(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq;

    eq = party_find_equipment(item_id);
    if (eq == 0) return "なし";
    return eq->name;
}

static UINT8 party_equipped_weapon_type(PartyMemberRuntime *member) BANKED {
    const PartyEquipmentDef *eq;

    if (member == 0) return PARTY_WEAPON_NONE;
    eq = party_find_equipment(member->weapon_id);
    if (eq == 0) return member->weapon_type;
    return eq->weapon_type;
}

static UINT8 party_equipment_attack(PartyMemberRuntime *member) BANKED {
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    eq = party_find_equipment(member->weapon_id);
    if (eq == 0) return 0u;
    return eq->attack;
}

static UINT8 party_equipment_defense(PartyMemberRuntime *member) BANKED {
    UINT8 v;
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    v = 0u;
    eq = party_find_equipment(member->armor_id);
    if (eq != 0) v = (UINT8)(v + eq->defense);
    eq = party_find_equipment(member->accessory_id);
    if (eq != 0) v = (UINT8)(v + eq->defense);
    return v;
}

static UINT8 party_equipment_skill(PartyMemberRuntime *member) BANKED {
    UINT8 v;
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    v = 0u;
    eq = party_find_equipment(member->weapon_id);
    if (eq != 0) v = (UINT8)(v + eq->skill);
    eq = party_find_equipment(member->accessory_id);
    if (eq != 0) v = (UINT8)(v + eq->skill);
    return v;
}

static UINT8 party_equipment_heal(PartyMemberRuntime *member) BANKED {
    UINT8 v;
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    v = 0u;
    eq = party_find_equipment(member->weapon_id);
    if (eq != 0) v = (UINT8)(v + eq->heal);
    eq = party_find_equipment(member->accessory_id);
    if (eq != 0) v = (UINT8)(v + eq->heal);
    return v;
}

static UINT8 party_equipment_agility(PartyMemberRuntime *member) BANKED {
    UINT8 v;
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    v = 0u;
    eq = party_find_equipment(member->weapon_id);
    if (eq != 0) v = (UINT8)(v + eq->agility);
    eq = party_find_equipment(member->accessory_id);
    if (eq != 0) v = (UINT8)(v + eq->agility);
    return v;
}

static UINT8 party_equipment_weight(PartyMemberRuntime *member) BANKED {
    UINT8 v;
    const PartyEquipmentDef *eq;

    if (member == 0) return 0u;
    v = member->equip_weight;
    eq = party_find_equipment(member->weapon_id);
    if (eq != 0) v = (UINT8)(v + eq->weight);
    eq = party_find_equipment(member->armor_id);
    if (eq != 0) v = (UINT8)(v + eq->weight);
    eq = party_find_equipment(member->accessory_id);
    if (eq != 0) v = (UINT8)(v + eq->weight);
    return v;
}

static const char *party_weapon_type_name(UINT8 weapon_type) BANKED {
    switch (weapon_type) {
        case PARTY_WEAPON_SWORD: return "剣";
        case PARTY_WEAPON_STAFF: return "杖";
        case PARTY_WEAPON_BOW: return "弓";
        case PARTY_WEAPON_GLOVE: return "体術";
        case PARTY_WEAPON_TOOL: return "道具";
        default: return "なし";
    }
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
    weapon_type = party_equipped_weapon_type(member);
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
    value += party_equipment_attack(member);
    value += party_mastery_bonus(member);
    value += party_morale_bonus(member);
    if (member->learned_tech_flags & PARTY_TECH_FLAG_RANK1) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_RANK2) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_COUNTER) value++;
    if (member->formation_role == PARTY_FORM_BACK && party_equipped_weapon_type(member) != PARTY_WEAPON_BOW) {
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
    value += party_equipment_defense(member);
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
    value += party_equipment_agility(member);
    if (member->formation_role == PARTY_FORM_SWIFT) value += 2u;
    if (member->formation_role == PARTY_FORM_FOCUS && member->focus >= 16u) value++;
    value += party_sense_bonus(member);
    penalty = (UINT8)((party_equipment_weight(member) >> 1) + party_fatigue_penalty(member));
    if (value > penalty) value -= penalty;
    else value = 1u;
    if (member->status_flags & PARTY_STATUS_SLEEP) value = 1u;
    return party_clamp_u8(value);
}

static UINT8 party_effective_skill(PartyMemberRuntime *member) BANKED {
    UINT16 value;

    if (member == 0) return 0u;
    value = member->skill_power;
    value += party_equipment_skill(member);
    value += party_mastery_bonus(member);
    value += party_focus_bonus(member);
    if (member->race_type == PARTY_RACE_MUTANT) value++;
    if (member->learned_magic_flags & (PARTY_MAGIC_FLAG_FIRE | PARTY_MAGIC_FLAG_ICE | PARTY_MAGIC_FLAG_THUNDER)) value++;
    if (member->learned_tech_flags & PARTY_TECH_FLAG_SPARK) value++;
    if (party_equipped_weapon_type(member) == PARTY_WEAPON_TOOL && (member->learned_tech_flags & PARTY_TECH_FLAG_TOOL)) value++;
    return party_clamp_u8(value);
}

static UINT8 party_effective_heal(PartyMemberRuntime *member) BANKED {
    UINT16 value;

    if (member == 0) return 0u;
    value = member->heal_power;
    value += party_equipment_heal(member);
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

    weapon_type = party_equipped_weapon_type(member);
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


static UINT8 party_growth_roll(UINT8 *seed) BANKED {
    *seed = (UINT8)((*seed * 17u) + 29u);
    return *seed;
}

static void party_post_battle_one_growth(PartyMemberRuntime *member, UINT8 enemy_rank, UINT8 *seed) BANKED {
    UINT8 chance;
    UINT8 roll;
    UINT8 style;
    UINT8 weapon_type;

    if (member == 0) return;
    if (member->hp == 0u) {
        party_add_capped(&member->fatigue, 1u, PARTY_FATIGUE_MAX);
        member->last_action_style = PARTY_ACTION_NONE;
        return;
    }

    if (enemy_rank == 0u) enemy_rank = 1u;
    if (enemy_rank > 5u) enemy_rank = 5u;

    style = member->last_action_style;
    chance = (UINT8)(28u + (UINT8)(enemy_rank * 5u));
    if (member->morale >= 16u) chance = (UINT8)(chance + 5u);
    if (member->focus >= 16u) chance = (UINT8)(chance + 5u);
    if (member->fatigue >= 24u && chance > 10u) chance = (UINT8)(chance - 10u);

    roll = (UINT8)(party_growth_roll(seed) % 100u);
    if (roll < chance) {
        if (style == PARTY_ACTION_DAMAGE) {
            if (member->max_hp < 199u) member->max_hp++;
            if (member->defense < 99u && (party_growth_roll(seed) & 3u) == 0u) member->defense++;
        } else if (style == PARTY_ACTION_HEAL) {
            if (member->heal_power < 99u) member->heal_power++;
            if (member->max_mp < 99u && (party_growth_roll(seed) & 1u) != 0u) member->max_mp++;
        } else if (style == PARTY_ACTION_SKILL) {
            if (member->skill_power < 99u) member->skill_power++;
            if (member->max_mp < 99u && (party_growth_roll(seed) & 1u) != 0u) member->max_mp++;
        } else if (style == PARTY_ACTION_ATTACK) {
            if (member->attack < 99u) member->attack++;
            if (member->agility < 99u && (party_growth_roll(seed) & 3u) == 0u) member->agility++;
        } else {
            if (member->max_hp < 199u) member->max_hp++;
        }
    }

    weapon_type = party_equipped_weapon_type(member);
    if (style == PARTY_ACTION_ATTACK && weapon_type < PARTY_WEAPON_COUNT && member->weapon_mastery[weapon_type] < PARTY_MASTERY_MAX) {
        if ((party_growth_roll(seed) & 1u) != 0u) {
            party_gain_mastery(member, PARTY_TECH_FLAG_NONE);
        }
    }

    if (member->adventure_sense < PARTY_SENSE_MAX && (party_growth_roll(seed) % 100u) < (UINT8)(12u + enemy_rank)) {
        member->adventure_sense++;
    }

    if (member->fatigue > 0u && (party_growth_roll(seed) & 1u) != 0u) member->fatigue--;
    if (member->morale > 0u && member->morale < PARTY_MORALE_MAX) member->morale++;
    member->last_action_style = PARTY_ACTION_NONE;
}

void party_after_battle_growth(UINT8 enemy_rank, UINT8 random_seed) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        party_post_battle_one_growth(member, enemy_rank, &random_seed);
    }
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

    if (op == PARTY_OP_NOTE_ATTACK) {
        party_gain_mastery(member, PARTY_TECH_FLAG_NONE);
        party_note_action(member, PARTY_ACTION_ATTACK);
        if (member->focus < PARTY_FOCUS_MAX && (member->weapon_mastery[party_equipped_weapon_type(member)] & 3u) == 0u) {
            member->focus++;
        }
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

UINT16 party_heal_active(UINT8 active_slot, UINT16 amount) BANKED {
    UINT16 after;
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;
    if (member->hp == 0u) return member->hp;

    after = (UINT16)(member->hp + amount);
    if (after > member->max_hp || after < member->hp) {
        after = member->max_hp;
    }
    member->hp = after;
    member->status_flags &= (UINT8)~(PARTY_STATUS_SLEEP | PARTY_STATUS_STUN | PARTY_STATUS_DOWN | PARTY_STATUS_TIRED);
    if (member->fatigue > 1u) member->fatigue = (UINT8)(member->fatigue - 2u);
    else member->fatigue = 0u;
    return member->hp;
}

void party_heal_all_active(void) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member == 0) continue;
        member->hp = member->max_hp;
        member->mp = member->max_mp;
        member->status_flags &= (UINT8)~(PARTY_STATUS_POISON | PARTY_STATUS_SLEEP | PARTY_STATUS_STUN | PARTY_STATUS_DOWN | PARTY_STATUS_TIRED);
        if (member->fatigue > 2u) member->fatigue = (UINT8)(member->fatigue - 3u);
        else member->fatigue = 0u;
    }
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


static void party_ui_clear(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
}

static void party_put_field_text(UINT8 x, UINT8 y, UINT8 w, const char *text) BANKED {
    jp_bkg_clear_area(x, y, w, 1u);
    jp_put_bkg_text(x, y, text);
}

static void party_u8_to_dec(UINT8 value, char *out) BANKED {
    UINT8 i;
    UINT8 tens;

    i = 0u;
    if (value >= 100u) {
        out[i++] = (char)('0' + (value / 100u));
        value = (UINT8)(value % 100u);
        out[i++] = (char)('0' + (value / 10u));
    } else if (value >= 10u) {
        tens = (UINT8)(value / 10u);
        out[i++] = (char)('0' + tens);
    }
    out[i++] = (char)('0' + (value % 10u));
    out[i] = '\0';
}

static void party_u16_to_dec3(UINT16 value, char *out) BANKED {
    UINT8 i;
    UINT8 hundreds;
    UINT8 tens;

    if (value > 999u) value = 999u;
    i = 0u;
    hundreds = (UINT8)(value / 100u);
    tens = (UINT8)((value / 10u) % 10u);
    if (hundreds != 0u) {
        out[i++] = (char)('0' + hundreds);
        out[i++] = (char)('0' + tens);
    } else if (tens != 0u) {
        out[i++] = (char)('0' + tens);
    }
    out[i++] = (char)('0' + (UINT8)(value % 10u));
    out[i] = '\0';
}

static void party_put_u8(UINT8 x, UINT8 y, UINT8 value) BANKED {
    char buf[4];
    party_u8_to_dec(value, buf);
    jp_put_bkg_text(x, y, buf);
}

static void party_put_u16(UINT8 x, UINT8 y, UINT16 value) BANKED {
    char buf[4];
    party_u16_to_dec3(value, buf);
    jp_put_bkg_text(x, y, buf);
}

UINT8 party_use_field_item_on_active(UINT8 item_id, UINT8 active_slot) BANKED {
    PartyMemberRuntime *member;
    UINT16 heal;

    member = party_get_active_member(active_slot);
    if (member == 0) return 0u;

    switch (item_id) {
        case ITEM_HERB:
            if (member->hp >= member->max_hp) return 0u;
            heal = 30u;
            member->hp = (UINT16)((member->hp + heal > member->max_hp) ? member->max_hp : (member->hp + heal));
            return 1u;
        case ITEM_POTION:
            if (member->hp >= member->max_hp) return 0u;
            heal = 60u;
            member->hp = (UINT16)((member->hp + heal > member->max_hp) ? member->max_hp : (member->hp + heal));
            return 1u;
        case ITEM_MANA_HERB:
            if (member->mp >= member->max_mp) return 0u;
            heal = 10u;
            member->mp = (UINT16)((member->mp + heal > member->max_mp) ? member->max_mp : (member->mp + heal));
            return 1u;
        case ITEM_ANTIDOTE:
            if ((member->status_flags & PARTY_STATUS_POISON) == 0u) return 0u;
            member->status_flags &= (UINT8)~PARTY_STATUS_POISON;
            return 1u;
        default:
            return 0u;
    }
}

static void party_draw_status_page(UINT8 active_slot) BANKED {
    PartyBattleFighter f;
    PartyMemberRuntime *member;

    party_get_active_fighter(active_slot, &f);
    member = party_get_active_member(active_slot);

    party_ui_clear();
    jp_draw_bkg_frame(0u, 0u, 20u, 15u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, "つよさ");
    jp_put_bkg_text(13u, 1u, "< >");
    party_put_field_text(1u, 2u, 10u, f.name);
    jp_put_bkg_text(1u, 3u, "HP"); party_put_u16(6u, 3u, f.hp); jp_put_bkg_text(9u, 3u, "/"); party_put_u16(11u, 3u, f.max_hp);
    jp_put_bkg_text(1u, 4u, "MP"); party_put_u16(6u, 4u, f.mp); jp_put_bkg_text(9u, 4u, "/"); party_put_u16(11u, 4u, f.max_mp);
    jp_put_bkg_text(1u, 6u, "ちから"); party_put_u8(12u, 6u, f.attack);
    jp_put_bkg_text(1u, 7u, "まもり"); party_put_u8(12u, 7u, f.defense);
    jp_put_bkg_text(1u, 8u, "とくぎ"); party_put_u8(12u, 8u, f.skill_power);
    jp_put_bkg_text(1u, 9u, "かいふく"); party_put_u8(12u, 9u, f.heal_power);
    jp_put_bkg_text(1u,10u, "すばやさ"); party_put_u8(12u,10u, f.agility);
    if (member != 0) {
        jp_put_bkg_text(1u,11u, "武器"); party_put_field_text(7u,11u, 5u, party_weapon_type_name(party_equipped_weapon_type(member)));
        jp_put_bkg_text(1u,12u, "熟練"); party_put_u8(7u,12u, member->weapon_mastery[party_equipped_weapon_type(member)]);
        jp_put_bkg_text(10u,12u, "勘"); party_put_u8(14u,12u, member->adventure_sense);
        jp_put_bkg_text(1u,13u, "戦意"); party_put_u8(7u,13u, member->morale);
        jp_put_bkg_text(10u,13u, "集中"); party_put_u8(16u,13u, member->focus);
        jp_put_bkg_text(1u,14u, "疲労"); party_put_u8(7u,14u, member->fatigue);
    }
    jp_put_bkg_text(1u,16u, "< > なかま  A/Bもどる");
}

void party_menu_show_heal_skill_loop(void) BANKED {
    UINT8 keys;
    UINT8 target_slot;
    UINT8 caster_slot;
    UINT8 phase;
    PartyBattleFighter caster;
    PartyBattleFighter target;
    UINT16 amount;

    caster_slot = (PARTY_ACTIVE_COUNT > 1u) ? 1u : 0u;
    target_slot = 0u;
    phase = 0u;
    party_ui_clear();
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);

    while (1) {
        if (phase == 0u) {
            jp_bkg_clear_area(1u, 1u, 18u, 15u);
            jp_put_bkg_text(1u, 1u, "特技一覧");
            jp_put_bkg_text(1u, 4u, ">");
            jp_put_bkg_text(3u, 4u, "かいふく");
            jp_put_bkg_text(1u, 7u, "MP4 味方HP回復");
            jp_put_bkg_text(1u,16u, "A選択 B戻る");
            keys = audio_waitpad(J_A | J_B | J_START);
            audio_waitpadup();
            if (keys & (J_B | J_START)) break;
            if (keys & J_A) {
                phase = 1u;
                target_slot = 0u;
            }
        } else {
            party_get_active_fighter(target_slot, &target);
            jp_bkg_clear_area(1u, 1u, 18u, 15u);
            jp_put_bkg_text(1u, 1u, "かいふく 対象");
            jp_put_bkg_text(1u, 3u, ">");
            jp_put_bkg_text(3u, 3u, target.name);
            jp_put_bkg_text(1u, 5u, "HP"); party_put_u16(5u, 5u, target.hp); jp_put_bkg_text(8u,5u,"/"); party_put_u16(10u,5u,target.max_hp);
            jp_put_bkg_text(1u, 7u, "MP4 で回復");
            jp_put_bkg_text(1u,16u, "上下 A使用 B一覧");

            keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
            audio_waitpadup();
            if (keys & J_UP) {
                if (target_slot == 0u) target_slot = PARTY_ACTIVE_COUNT - 1u;
                else target_slot--;
            } else if (keys & J_DOWN) {
                target_slot++;
                if (target_slot >= PARTY_ACTIVE_COUNT) target_slot = 0u;
            } else if (keys & J_B) {
                phase = 0u;
            } else if (keys & J_START) {
                break;
            } else if (keys & J_A) {
                party_get_active_fighter(caster_slot, &caster);
                if (caster.hp == 0u || caster.mp < 4u || target.hp >= target.max_hp) {
                    jp_bkg_clear_area(1u, 12u, 18u, 1u);
                    jp_put_bkg_text(1u, 12u, "使えません");
                } else {
                    party_battle_op(PARTY_OP_TRY_CONSUME_MP, caster_slot, 4u);
                    amount = (UINT16)((UINT16)caster.heal_power * 2u + 6u);
                    party_heal_active(target_slot, amount);
                    jp_bkg_clear_area(1u, 12u, 18u, 1u);
                    jp_put_bkg_text(1u, 12u, "回復しました");
                }
            }
        }
    }
}


void party_menu_show_status_loop(void) BANKED {
    UINT8 keys;
    UINT8 slot;

    slot = 0u;
    audio_waitpadup();
    party_draw_status_page(slot);
    while (1) {
        keys = audio_waitpad(J_LEFT | J_RIGHT | J_A | J_B | J_START);
        audio_waitpadup();
        if (keys & J_LEFT) {
            if (slot == 0u) slot = PARTY_ACTIVE_COUNT - 1u;
            else slot--;
            party_draw_status_page(slot);
        } else if (keys & J_RIGHT) {
            slot++;
            if (slot >= PARTY_ACTIVE_COUNT) slot = 0u;
            party_draw_status_page(slot);
        } else {
            break;
        }
    }
}

static UINT8 party_member_slot_item(PartyMemberRuntime *member, UINT8 slot) BANKED {
    if (member == 0) return ITEM_NONE;
    if (slot == PARTY_EQUIP_SLOT_WEAPON) return member->weapon_id;
    if (slot == PARTY_EQUIP_SLOT_ARMOR) return member->armor_id;
    return member->accessory_id;
}

static void party_member_set_slot_item(PartyMemberRuntime *member, UINT8 slot, UINT8 item_id) BANKED {
    if (member == 0) return;
    if (slot == PARTY_EQUIP_SLOT_WEAPON) member->weapon_id = item_id;
    else if (slot == PARTY_EQUIP_SLOT_ARMOR) member->armor_id = item_id;
    else member->accessory_id = item_id;
    member->weapon_type = party_equipped_weapon_type(member);
}

static UINT8 party_build_equip_candidates(PartyMemberRuntime *member, UINT8 slot, UINT8 *out) BANKED {
    UINT8 count;
    UINT8 i;
    UINT8 current;
    const PartyEquipmentDef *eq;

    if (out == 0) return 0u;
    count = 0u;
    out[count++] = ITEM_NONE;
    current = party_member_slot_item(member, slot);
    if (current != ITEM_NONE) out[count++] = current;
    for (i = 0u; i < PARTY_EQUIPMENT_DEF_COUNT && count < 12u; i++) {
        eq = &party_equipment_defs[i];
        if (eq->slot != slot) continue;
        if (eq->item_id == current) continue;
        if (inventory_get_count(eq->item_id) == 0u) continue;
        out[count++] = eq->item_id;
    }
    return count;
}

static UINT8 party_apply_equip_choice(PartyMemberRuntime *member, UINT8 slot, UINT8 chosen_item) BANKED {
    UINT8 old_item;

    if (member == 0) return 0u;
    old_item = party_member_slot_item(member, slot);
    if (old_item == chosen_item) return 1u;
    if (chosen_item != ITEM_NONE && inventory_has(chosen_item, 1u) == 0u) return 0u;
    if (chosen_item != ITEM_NONE) inventory_remove(chosen_item, 1u);
    if (old_item != ITEM_NONE) inventory_add(old_item, 1u);
    party_member_set_slot_item(member, slot, chosen_item);
    return 1u;
}

static void party_draw_equip_popup(PartyMemberRuntime *member, UINT8 slot_cursor, UINT8 item_cursor, const UINT8 *items, UINT8 item_count) BANKED {
    UINT8 i;
    UINT8 y;
    (void)member;
    jp_draw_bkg_frame(10u, 3u, 10u, 10u);
    party_put_field_text(11u, 4u, 8u, (slot_cursor == PARTY_EQUIP_SLOT_WEAPON) ? "武器" : (slot_cursor == PARTY_EQUIP_SLOT_ARMOR) ? "防具" : "装飾");
    y = 6u;
    for (i = 0u; i < 6u; i++, y++) {
        if (i < item_count) {
            jp_put_bkg_text(11u, y, (i == item_cursor) ? ">" : " ");
            party_put_field_text(12u, y, 7u, party_equipment_name(items[i]));
        } else {
            party_put_field_text(11u, y, 8u, "");
        }
    }
}

static void party_draw_equip_page(UINT8 active_slot, UINT8 slot_cursor, UINT8 choosing, UINT8 item_cursor, const UINT8 *items, UINT8 item_count, const char *message) BANKED {
    PartyMemberRuntime *member;

    member = party_get_active_member(active_slot);
    party_ui_clear();
    jp_draw_bkg_frame(0u, 0u, 20u, 15u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, "そうび");
    jp_put_bkg_text(13u, 1u, "< >");
    if (member == 0) {
        jp_put_bkg_text(2u, 4u, "なかま なし");
        party_put_field_text(1u, 16u, 18u, message);
        return;
    }
    party_put_field_text(1u, 3u, 10u, member->name);
    jp_put_bkg_text(1u, 5u, (slot_cursor == PARTY_EQUIP_SLOT_WEAPON) ? ">" : " "); jp_put_bkg_text(2u, 5u, "武器"); party_put_field_text(6u, 5u, 9u, party_equipment_name(member->weapon_id));
    jp_put_bkg_text(1u, 6u, (slot_cursor == PARTY_EQUIP_SLOT_ARMOR) ? ">" : " "); jp_put_bkg_text(2u, 6u, "防具"); party_put_field_text(6u, 6u, 9u, party_equipment_name(member->armor_id));
    jp_put_bkg_text(1u, 7u, (slot_cursor == PARTY_EQUIP_SLOT_ACC) ? ">" : " "); jp_put_bkg_text(2u, 7u, "装飾"); party_put_field_text(6u, 7u, 9u, party_equipment_name(member->accessory_id));
    jp_put_bkg_text(1u, 9u,  "武器種"); party_put_field_text(8u, 9u, 5u, party_weapon_type_name(party_equipped_weapon_type(member)));
    jp_put_bkg_text(1u, 10u, "攻撃+"); party_put_u8(8u,10u, party_equipment_attack(member));
    jp_put_bkg_text(1u, 11u, "防御+"); party_put_u8(8u,11u, party_equipment_defense(member));
    jp_put_bkg_text(1u, 12u, "重量");  party_put_u8(8u,12u, party_equipment_weight(member));
    party_put_field_text(1u, 16u, 18u, message);
    if (choosing != 0u) party_draw_equip_popup(member, slot_cursor, item_cursor, items, item_count);
}

static void party_update_equip_slot_cursor(UINT8 slot_cursor) BANKED {
    jp_put_bkg_text(1u, 5u, (slot_cursor == PARTY_EQUIP_SLOT_WEAPON) ? ">" : " ");
    jp_put_bkg_text(1u, 6u, (slot_cursor == PARTY_EQUIP_SLOT_ARMOR) ? ">" : " ");
    jp_put_bkg_text(1u, 7u, (slot_cursor == PARTY_EQUIP_SLOT_ACC) ? ">" : " ");
}

static void party_update_equip_popup_cursor(UINT8 item_cursor, const UINT8 *items, UINT8 item_count) BANKED {
    UINT8 i;
    UINT8 y;
    (void)items;
    y = 6u;
    for (i = 0u; i < 6u; i++, y++) {
        if (i < item_count) {
            jp_put_bkg_text(11u, y, (i == item_cursor) ? ">" : " ");
        }
    }
}

void party_menu_show_equip_loop(void) BANKED {
    UINT8 keys;
    UINT8 slot;
    UINT8 slot_cursor;
    UINT8 choosing;
    UINT8 item_cursor;
    UINT8 old_item_cursor;
    UINT8 item_count;
    UINT8 item_choices[12];
    const char *message;
    PartyMemberRuntime *member;
    UINT8 redraw_page;

    slot = 0u;
    slot_cursor = PARTY_EQUIP_SLOT_WEAPON;
    choosing = 0u;
    item_cursor = 0u;
    item_count = 0u;
    message = "A=えらぶ B=もどる";
    audio_waitpadup();
    party_draw_equip_page(slot, slot_cursor, choosing, item_cursor, item_choices, item_count, message);
    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START);
        audio_waitpadup();
        member = party_get_active_member(slot);
        redraw_page = 0u;
        if (choosing == 0u) {
            if (keys & J_UP) {
                if (slot_cursor == 0u) slot_cursor = PARTY_EQUIP_SLOT_ACC;
                else slot_cursor--;
                party_update_equip_slot_cursor(slot_cursor);
            } else if (keys & J_DOWN) {
                slot_cursor++;
                if (slot_cursor > PARTY_EQUIP_SLOT_ACC) slot_cursor = PARTY_EQUIP_SLOT_WEAPON;
                party_update_equip_slot_cursor(slot_cursor);
            } else if (keys & J_LEFT) {
                if (slot == 0u) slot = PARTY_ACTIVE_COUNT - 1u;
                else slot--;
                redraw_page = 1u;
            } else if (keys & J_RIGHT) {
                slot++;
                if (slot >= PARTY_ACTIVE_COUNT) slot = 0u;
                redraw_page = 1u;
            } else if (keys & (J_B | J_START)) {
                break;
            } else if (keys & J_A) {
                item_count = party_build_equip_candidates(member, slot_cursor, item_choices);
                item_cursor = 0u;
                choosing = 1u;
                message = "A=そうび B=やめる";
                redraw_page = 1u;
            }
        } else {
            if (keys & J_UP) {
                old_item_cursor = item_cursor;
                if (item_cursor == 0u) item_cursor = (UINT8)(item_count - 1u);
                else item_cursor--;
                if (old_item_cursor != item_cursor) party_update_equip_popup_cursor(item_cursor, item_choices, item_count);
            } else if (keys & J_DOWN) {
                old_item_cursor = item_cursor;
                item_cursor++;
                if (item_cursor >= item_count) item_cursor = 0u;
                if (old_item_cursor != item_cursor) party_update_equip_popup_cursor(item_cursor, item_choices, item_count);
            } else if (keys & J_B) {
                choosing = 0u;
                message = "A=えらぶ B=もどる";
                redraw_page = 1u;
            } else if (keys & J_A) {
                if (party_apply_equip_choice(member, slot_cursor, item_choices[item_cursor])) message = "そうびした";
                else message = "そうびできない";
                choosing = 0u;
                redraw_page = 1u;
            }
        }
        if (redraw_page != 0u) {
            party_draw_equip_page(slot, slot_cursor, choosing, item_cursor, item_choices, item_count, message);
        }
    }
}

void party_save_copy_to(PartySaveState *dst) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    if (dst == 0) return;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member == 0) {
            dst->active[i].max_hp = 0u;
            dst->active[i].hp = 0u;
            dst->active[i].max_mp = 0u;
            dst->active[i].mp = 0u;
            dst->active[i].attack = 0u;
            dst->active[i].defense = 0u;
            dst->active[i].skill_power = 0u;
            dst->active[i].heal_power = 0u;
            dst->active[i].agility = 0u;
            dst->active[i].weapon_id = ITEM_NONE;
            dst->active[i].armor_id = ITEM_NONE;
            dst->active[i].accessory_id = ITEM_NONE;
            dst->active[i].weapon_mastery[0] = 0u;
            dst->active[i].weapon_mastery[1] = 0u;
            dst->active[i].weapon_mastery[2] = 0u;
            dst->active[i].weapon_mastery[3] = 0u;
            dst->active[i].weapon_mastery[4] = 0u;
            dst->active[i].weapon_mastery[5] = 0u;
            dst->active[i].learned_tech_flags = 0u;
            dst->active[i].learned_magic_flags = 0u;
            dst->active[i].morale = 0u;
            dst->active[i].focus = 0u;
            dst->active[i].adventure_sense = 0u;
            dst->active[i].fatigue = 0u;
        } else {
            dst->active[i].max_hp = member->max_hp;
            dst->active[i].hp = member->hp;
            dst->active[i].max_mp = member->max_mp;
            dst->active[i].mp = member->mp;
            dst->active[i].attack = member->attack;
            dst->active[i].defense = member->defense;
            dst->active[i].skill_power = member->skill_power;
            dst->active[i].heal_power = member->heal_power;
            dst->active[i].agility = member->agility;
            dst->active[i].weapon_id = member->weapon_id;
            dst->active[i].armor_id = member->armor_id;
            dst->active[i].accessory_id = member->accessory_id;
            dst->active[i].weapon_mastery[0] = member->weapon_mastery[0];
            dst->active[i].weapon_mastery[1] = member->weapon_mastery[1];
            dst->active[i].weapon_mastery[2] = member->weapon_mastery[2];
            dst->active[i].weapon_mastery[3] = member->weapon_mastery[3];
            dst->active[i].weapon_mastery[4] = member->weapon_mastery[4];
            dst->active[i].weapon_mastery[5] = member->weapon_mastery[5];
            dst->active[i].learned_tech_flags = member->learned_tech_flags;
            dst->active[i].learned_magic_flags = member->learned_magic_flags;
            dst->active[i].morale = member->morale;
            dst->active[i].focus = member->focus;
            dst->active[i].adventure_sense = member->adventure_sense;
            dst->active[i].fatigue = member->fatigue;
        }
    }
}

void party_save_copy_from(const PartySaveState *src) BANKED {
    UINT8 i;
    PartyMemberRuntime *member;

    if (src == 0) return;

    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        member = party_get_active_member(i);
        if (member == 0) continue;
        member->max_hp = src->active[i].max_hp;
        member->hp = src->active[i].hp;
        member->max_mp = src->active[i].max_mp;
        member->mp = src->active[i].mp;
        member->attack = src->active[i].attack;
        member->defense = src->active[i].defense;
        member->skill_power = src->active[i].skill_power;
        member->heal_power = src->active[i].heal_power;
        member->agility = src->active[i].agility;
        member->weapon_id = src->active[i].weapon_id;
        member->armor_id = src->active[i].armor_id;
        member->accessory_id = src->active[i].accessory_id;
        member->weapon_mastery[0] = src->active[i].weapon_mastery[0];
        member->weapon_mastery[1] = src->active[i].weapon_mastery[1];
        member->weapon_mastery[2] = src->active[i].weapon_mastery[2];
        member->weapon_mastery[3] = src->active[i].weapon_mastery[3];
        member->weapon_mastery[4] = src->active[i].weapon_mastery[4];
        member->weapon_mastery[5] = src->active[i].weapon_mastery[5];
        member->learned_tech_flags = src->active[i].learned_tech_flags;
        member->learned_magic_flags = src->active[i].learned_magic_flags;
        member->morale = src->active[i].morale;
        member->focus = src->active[i].focus;
        member->adventure_sense = src->active[i].adventure_sense;
        member->fatigue = src->active[i].fatigue;
    }
}
