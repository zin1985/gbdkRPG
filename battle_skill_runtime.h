#ifndef BATTLE_SKILL_RUNTIME_H
#define BATTLE_SKILL_RUNTIME_H

#include <gb/gb.h>

#define BATTLE_SKILL_EVENT_NONE        0u
#define BATTLE_SKILL_EVENT_CANCEL      1u
#define BATTLE_SKILL_EVENT_ENEMY_SKILL 2u
#define BATTLE_SKILL_EVENT_ALLY_SKILL  3u

void battle_skill_runtime_start(UINT8 party_turn_slot) BANKED;
UINT8 battle_skill_runtime_update(UINT8 *skill_id) BANKED;
void battle_skill_runtime_start_ally(UINT8 skill_id) BANKED;
UINT8 battle_skill_runtime_update_ally(UINT8 *skill_id, UINT8 *ally_slot) BANKED;

#endif
