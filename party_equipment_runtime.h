#ifndef PARTY_EQUIPMENT_RUNTIME_H
#define PARTY_EQUIPMENT_RUNTIME_H

#include <gb/gb.h>

UINT8 party_equipment_data_count(void) BANKED;
UINT8 party_equipment_data_item_at(UINT8 index) BANKED;
UINT8 party_equipment_data_exists(UINT8 item_id) BANKED;
UINT8 party_equipment_data_slot(UINT8 item_id) BANKED;
UINT8 party_equipment_data_weapon_type(UINT8 item_id) BANKED;
UINT8 party_equipment_data_attack(UINT8 item_id) BANKED;
UINT8 party_equipment_data_defense(UINT8 item_id) BANKED;
UINT8 party_equipment_data_skill(UINT8 item_id) BANKED;
UINT8 party_equipment_data_heal(UINT8 item_id) BANKED;
UINT8 party_equipment_data_agility(UINT8 item_id) BANKED;
UINT8 party_equipment_data_weight(UINT8 item_id) BANKED;

UINT8 party_equipment_build_candidates(UINT8 current, UINT8 slot, UINT8 *out, UINT8 max_count) BANKED;

#endif
