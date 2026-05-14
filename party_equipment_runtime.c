#pragma bank 25

#include <gb/gb.h>
#include "inventory.h"
#include "party_equipment_runtime.h"
#include "menu_text_dict.h"
#include "jpfont.h"

BANKREF(party_equipment_runtime_bank)

#ifndef PARTY_WEAPON_COUNT
#define PARTY_WEAPON_NONE  0u
#define PARTY_WEAPON_SWORD 1u
#define PARTY_WEAPON_STAFF 2u
#define PARTY_WEAPON_BOW   3u
#define PARTY_WEAPON_GLOVE 4u
#define PARTY_WEAPON_TOOL  5u
#define PARTY_WEAPON_SPEAR 6u
#define PARTY_WEAPON_AXE   7u
#define PARTY_WEAPON_COUNT 8u
#endif

#define PARTY_EQUIP_SLOT_WEAPON 0u
#define PARTY_EQUIP_SLOT_ARMOR  1u
#define PARTY_EQUIP_SLOT_ACC    2u

#define PARTY_STATUS_POISON 0x01u
#define PARTY_STATUS_SLEEP  0x02u
#define PARTY_STATUS_STUN   0x04u
#define PARTY_STATUS_TIRED  0x08u
#define PARTY_STATUS_BLIND  0x10u
#define PARTY_STATUS_DOWN   0x80u

#define PARTY_ELEM_FIRE    0x01u
#define PARTY_ELEM_ICE     0x02u
#define PARTY_ELEM_THUNDER 0x04u
#define PARTY_ELEM_LIGHT   0x08u
#define PARTY_ELEM_DARK    0x10u


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
} PartyEquipmentDef;

static const PartyEquipmentDef party_equipment_defs[] = {
    { ITEM_WOOD_SWORD,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 2u, 0u, 0u, 0u, 0u, 1u, 0u, 0u },
    { ITEM_IRON_SWORD,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 4u, 0u, 0u, 0u, 0u, 3u, 0u, 0u },
    { ITEM_OAK_STAFF,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 1u, 0u, 0u, 2u, 0u, 1u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_MAGE_STAFF,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 1u, 0u, 3u, 1u, 0u, 1u, PARTY_ELEM_FIRE | PARTY_ELEM_ICE, 0u },
    { ITEM_SHORT_BOW,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   3u, 0u, 0u, 0u, 1u, 1u, PARTY_ELEM_THUNDER, 0u },
    { ITEM_GLOVES,        PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 2u, 0u, 1u, 0u, 2u, 0u, 0u, 0u },
    { ITEM_TOOL_KIT,      PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  1u, 0u, 2u, 0u, 0u, 1u, 0u, 0u },
    { ITEM_CLOTH_ARMOR,   PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 1u, 0u, 0u, 0u, 1u, 0u, 0u },
    { ITEM_LEATHER_ARMOR, PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 2u, 0u, 0u, 0u, 2u, 0u, 0u },
    { ITEM_IRON_ARMOR,    PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 4u, 0u, 0u, 0u, 5u, 0u, 0u },
    { ITEM_CHARM,         PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 1u, 1u, 0u, 0u, PARTY_ELEM_LIGHT, PARTY_STATUS_SLEEP },
    { ITEM_FEATHER,       PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 2u, 0u, 0u, 0u },
    { ITEM_DEBUG_NO_ENCOUNT, PARTY_EQUIP_SLOT_ACC, PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
    { ITEM_DEBUG_ESCAPE,  PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 2u, 0u, 0u, 0u },

    /* rpg182: additional equipment pool. */
    { ITEM_COPPER_SWORD,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 3u, 0u, 0u, 0u, 0u, 2u, 0u, 0u },
    { ITEM_STEEL_SWORD,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 5u, 0u, 0u, 0u, 0u, 4u, 0u, 0u },
    { ITEM_SILVER_SWORD,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 6u, 0u, 1u, 0u, 0u, 3u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_FLAME_SWORD,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 7u, 0u, 1u, 0u, 0u, 4u, PARTY_ELEM_FIRE, 0u },
    { ITEM_ICE_SWORD,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 7u, 0u, 1u, 0u, 0u, 4u, PARTY_ELEM_ICE, 0u },
    { ITEM_DRAGON_SWORD,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 9u, 1u, 2u, 0u, 0u, 5u, PARTY_ELEM_FIRE, 0u },
    { ITEM_WILLOW_STAFF,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 1u, 0u, 0u, 3u, 0u, 1u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_SAGE_STAFF,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 2u, 0u, 2u, 3u, 0u, 1u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_THUNDER_STAFF, PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 2u, 0u, 4u, 1u, 0u, 2u, PARTY_ELEM_THUNDER, 0u },
    { ITEM_MOON_STAFF,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 2u, 0u, 3u, 4u, 0u, 1u, PARTY_ELEM_LIGHT, PARTY_STATUS_SLEEP },
    { ITEM_SHADOW_STAFF,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_STAFF, 3u, 0u, 5u, 0u, 0u, 2u, PARTY_ELEM_DARK, 0u },
    { ITEM_LONG_BOW,      PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   4u, 0u, 0u, 0u, 1u, 2u, 0u, 0u },
    { ITEM_STEEL_BOW,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   5u, 0u, 0u, 0u, 1u, 3u, 0u, 0u },
    { ITEM_HUNTER_BOW,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   5u, 0u, 1u, 0u, 2u, 2u, 0u, 0u },
    { ITEM_THUNDER_BOW,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   6u, 0u, 1u, 0u, 2u, 3u, PARTY_ELEM_THUNDER, 0u },
    { ITEM_STAR_BOW,      PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_BOW,   7u, 0u, 2u, 1u, 3u, 2u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_BRONZE_SPEAR,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SPEAR, 4u, 0u, 0u, 0u, 1u, 2u, 0u, 0u },
    { ITEM_STEEL_SPEAR,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SPEAR, 6u, 0u, 1u, 0u, 1u, 3u, 0u, 0u },
    { ITEM_DRAGON_SPEAR,  PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SPEAR, 8u, 0u, 2u, 0u, 1u, 4u, PARTY_ELEM_THUNDER, 0u },
    { ITEM_HAND_AXE,      PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_AXE,   5u, 0u, 0u, 0u, 0u, 3u, 0u, 0u },
    { ITEM_BATTLE_AXE,    PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_AXE,   7u, 0u, 1u, 0u, 0u, 4u, 0u, 0u },
    { ITEM_GREAT_AXE,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_AXE,   9u, 0u, 2u, 0u, 0u, 5u, PARTY_ELEM_FIRE, 0u },
    { ITEM_IRON_GLOVES,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 3u, 0u, 1u, 0u, 2u, 1u, 0u, 0u },
    { ITEM_BEAST_CLAWS,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 4u, 0u, 2u, 0u, 3u, 1u, 0u, 0u },
    { ITEM_POWER_KNUCKLE, PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 6u, 0u, 2u, 0u, 1u, 2u, 0u, 0u },
    { ITEM_MONK_FIST,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_GLOVE, 5u, 0u, 3u, 1u, 4u, 0u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_ALCHEMY_KIT,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  2u, 0u, 3u, 0u, 0u, 1u, 0u, 0u },
    { ITEM_TRAP_TOOL,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  3u, 0u, 2u, 0u, 1u, 1u, 0u, 0u },
    { ITEM_BOMB_TOOL,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  5u, 0u, 3u, 0u, 0u, 2u, PARTY_ELEM_FIRE, 0u },
    { ITEM_GEAR_TOOL,     PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_TOOL,  4u, 0u, 4u, 0u, 1u, 2u, PARTY_ELEM_THUNDER, 0u },
    { ITEM_TRAVEL_CLOTH,  PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 1u, 0u, 0u, 1u, 0u, 0u, 0u },
    { ITEM_MAGE_ROBE,     PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 2u, 2u, 2u, 0u, 1u, PARTY_ELEM_ICE, 0u },
    { ITEM_BATTLE_CLOTH,  PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  1u, 3u, 1u, 0u, 1u, 1u, 0u, 0u },
    { ITEM_CHAIN_MAIL,    PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 5u, 0u, 0u, 0u, 4u, 0u, 0u },
    { ITEM_STEEL_ARMOR,   PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 6u, 0u, 0u, 0u, 6u, 0u, 0u },
    { ITEM_SILVER_ARMOR,  PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 6u, 1u, 1u, 0u, 4u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_DRAGON_ARMOR,  PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 8u, 1u, 0u, 0u, 7u, PARTY_ELEM_FIRE, 0u },
    { ITEM_QUICK_BOOTS,   PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 3u, 0u, 0u, 0u },
    { ITEM_POWER_RING,    PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u },
    { ITEM_GUARD_RING,    PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 2u, 0u, 0u, 0u, 0u, 0u, 0u },
    { ITEM_FOCUS_RING,    PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 2u, 0u, 1u, 0u, 0u, 0u },
    { ITEM_LUCKY_COIN,    PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 0u, 0u, 1u, 0u, 0u, 0u },
    { ITEM_DRAGON_SCALE,  PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  1u, 2u, 1u, 0u, 0u, 0u, PARTY_ELEM_FIRE, 0u },
    { ITEM_SLEEP_GUARD,   PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 1u, 0u, 0u, 0u, 0u, 0u, PARTY_STATUS_SLEEP },
    { ITEM_POISON_GUARD,  PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 1u, 0u, 0u, 0u, 0u, 0u, PARTY_STATUS_POISON },
    { ITEM_LIGHT_MEDAL,   PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 1u, 1u, 0u, 0u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_DARK_MEDAL,    PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  1u, 0u, 1u, 0u, 0u, 0u, PARTY_ELEM_DARK, 0u },
    { ITEM_ITIL_SWORD,   PARTY_EQUIP_SLOT_WEAPON, PARTY_WEAPON_SWORD, 100u, 0u, 0u, 0u, 0u, 1u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_ITIL_ARMOR,   PARTY_EQUIP_SLOT_ARMOR,  PARTY_WEAPON_NONE,  0u, 100u, 0u, 0u, 0u, 1u, PARTY_ELEM_LIGHT, 0u },
    { ITEM_ITIL_CREST,   PARTY_EQUIP_SLOT_ACC,    PARTY_WEAPON_NONE,  0u, 0u, 100u, 0u, 0u, 0u, PARTY_ELEM_LIGHT, 0u }
};
#define PARTY_EQUIPMENT_DEF_COUNT ((UINT8)(sizeof(party_equipment_defs) / sizeof(party_equipment_defs[0])))


static const PartyEquipmentDef *party_equipment_data_find(UINT8 item_id) {
    UINT8 i;
    if (item_id == ITEM_NONE) return 0;
    for (i = 0u; i < PARTY_EQUIPMENT_DEF_COUNT; i++) {
        if (party_equipment_defs[i].item_id == item_id) return &party_equipment_defs[i];
    }
    return 0;
}

UINT8 party_equipment_data_count(void) BANKED {
    return PARTY_EQUIPMENT_DEF_COUNT;
}

UINT8 party_equipment_data_item_at(UINT8 index) BANKED {
    if (index >= PARTY_EQUIPMENT_DEF_COUNT) return ITEM_NONE;
    return party_equipment_defs[index].item_id;
}

UINT8 party_equipment_data_exists(UINT8 item_id) BANKED {
    return (party_equipment_data_find(item_id) != 0) ? 1u : 0u;
}

UINT8 party_equipment_data_slot(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0xFFu;
    return eq->slot;
}

UINT8 party_equipment_data_weapon_type(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return PARTY_WEAPON_NONE;
    return eq->weapon_type;
}

UINT8 party_equipment_data_attack(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->attack;
}

UINT8 party_equipment_data_defense(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->defense;
}

UINT8 party_equipment_data_skill(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->skill;
}

UINT8 party_equipment_data_heal(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->heal;
}

UINT8 party_equipment_data_agility(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->agility;
}

UINT8 party_equipment_data_weight(UINT8 item_id) BANKED {
    const PartyEquipmentDef *eq = party_equipment_data_find(item_id);
    if (eq == 0) return 0u;
    return eq->weight;
}

static const char *party_equipment_weapon_type_name_local(UINT8 weapon_type) {
    switch (weapon_type) {
        case PARTY_WEAPON_SWORD: return "けん";
        case PARTY_WEAPON_STAFF: return "つえ";
        case PARTY_WEAPON_BOW: return "ゆみ";
        case PARTY_WEAPON_GLOVE: return "たい";
        case PARTY_WEAPON_TOOL: return "どうぐ";
        case PARTY_WEAPON_SPEAR: return "やり";
        case PARTY_WEAPON_AXE: return "おの";
        default: return "なし";
    }
}

static void party_equipment_put_field_text(UINT8 x, UINT8 y, UINT8 w, const char *text) {
    jp_bkg_clear_area(x, y, w, 1u);
    jp_put_bkg_text(x, y, text);
}

const char *party_equip_get_item_name(UINT8 item_id) BANKED {
    if (!party_equipment_data_exists(item_id)) return "なし";
    return menu_dict_item_name(item_id);
}

void party_equip_put_item_name(UINT8 x, UINT8 y, UINT8 width, UINT8 item_id) BANKED {
    if (!party_equipment_data_exists(item_id)) {
        party_equipment_put_field_text(x, y, width, "なし");
        return;
    }
    jp_bkg_clear_area(x, y, width, 1u);
    menu_dict_put_item_name(x, y, width, item_id);
}

UINT8 party_equip_get_item_attack_value(UINT8 item_id) BANKED {
    return party_equipment_data_attack(item_id);
}

UINT8 party_equip_get_item_defense_value(UINT8 item_id) BANKED {
    return party_equipment_data_defense(item_id);
}

UINT8 party_equip_get_item_weight_value(UINT8 item_id) BANKED {
    return party_equipment_data_weight(item_id);
}

UINT8 party_equip_get_item_weapon_type(UINT8 item_id) BANKED {
    return party_equipment_data_weapon_type(item_id);
}

void party_equip_put_weapon_type_value_name(UINT8 x, UINT8 y, UINT8 width, UINT8 weapon_type) BANKED {
    party_equipment_put_field_text(x, y, width, party_equipment_weapon_type_name_local(weapon_type));
}

UINT8 party_equipment_build_candidates(UINT8 current, UINT8 slot, UINT8 *out, UINT8 max_count) BANKED {
    UINT8 count;
    UINT8 i;
    UINT8 item_id;

    if (out == 0 || max_count == 0u) return 0u;
    count = 0u;
    out[count++] = ITEM_NONE;
    if (current != ITEM_NONE && count < max_count) out[count++] = current;

    for (i = 0u; i < party_equipment_data_count() && count < max_count; i++) {
        item_id = party_equipment_data_item_at(i);
        if (party_equipment_data_slot(item_id) != slot) continue;
        if (item_id == current) continue;
        if (inventory_get_count(item_id) == 0u) continue;
        out[count++] = item_id;
    }
    return count;
}
