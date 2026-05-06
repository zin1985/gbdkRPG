#ifndef GAME_FLAGS_H
#define GAME_FLAGS_H

#include <gb/gb.h>

#define GAME_FLAG_COUNT 512u
#define GAME_FLAG_BYTES 64u

void game_flags_clear_all(void);
void game_flag_set(UINT16 flag_id);
void game_flag_clear(UINT16 flag_id);
UINT8 game_flag_get(UINT16 flag_id);
void game_flags_copy_to(UINT8 *dst);
void game_flags_copy_from(const UINT8 *src);

#endif
