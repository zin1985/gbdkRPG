#ifndef BATTLE_STATUS_UI_RUNTIME_H
#define BATTLE_STATUS_UI_RUNTIME_H

#include <gb/gb.h>

BANKREF_EXTERN(battle_status_ui_runtime)

void battle_status_ui_runtime_clear_full(void) BANKED;
void battle_status_ui_runtime_draw_party_status_box(void) BANKED;

#endif
