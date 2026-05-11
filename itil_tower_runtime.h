#ifndef ITIL_TOWER_RUNTIME_H
#define ITIL_TOWER_RUNTIME_H

#include <gb/gb.h>

BANKREF_EXTERN(itil_tower_runtime_bank)

void itil_tower_start(void) BANKED;
void itil_tower_next_floor(void) BANKED;
UINT8 itil_tower_get_floor(void) BANKED;
UINT8 itil_tower_can_advance(void) BANKED;
UINT8 itil_tower_ask_question(void) BANKED;
UINT8 itil_tower_grant_reward(UINT8 random_value) BANKED;
void itil_tower_show_floor_banner(void) BANKED;
void itil_tower_show_need_answer(void) BANKED;
void itil_tower_show_wrong_exit(void) BANKED;
void itil_tower_show_reward(UINT8 item_id) BANKED;

#endif
