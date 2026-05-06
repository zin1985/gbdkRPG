#ifndef GAME_FLAGS_H
#define GAME_FLAGS_H

#include <gb/gb.h>

#define GAME_FLAG_COUNT 512u
#define GAME_FLAG_BYTES 64u

BANKREF_EXTERN(game_flags_bank)

void game_flags_clear_all(void) BANKED;
void game_flag_set(UINT16 flag_id) BANKED;
void game_flag_clear(UINT16 flag_id) BANKED;
UINT8 game_flag_get(UINT16 flag_id) BANKED;
void game_flags_copy_to(UINT8 *dst) BANKED;
void game_flags_copy_from(const UINT8 *src) BANKED;

#endif
