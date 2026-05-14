#ifndef EQUIP_RUNTIME_H
#define EQUIP_RUNTIME_H

#include <gb/gb.h>

BANKREF_EXTERN(equip_runtime_bank)

void equip_runtime_precache(void) BANKED;
void equip_runtime_show_loop(void) BANKED;

#endif
