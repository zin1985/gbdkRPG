#ifndef ACTOR_RUNTIME_H
#define ACTOR_RUNTIME_H

#include <gb/gb.h>

BANKREF_EXTERN(actor_runtime_bank)

void actor_runtime_init_actors(void) BANKED;
void actor_runtime_apply_area_npcs(UINT8 area) BANKED;

#endif
