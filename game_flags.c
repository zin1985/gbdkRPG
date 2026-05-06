#pragma bank 7

#include <gb/gb.h>
#include "game_flags.h"

BANKREF(game_flags_bank)

static UINT8 g_game_flags[GAME_FLAG_BYTES];

static UINT8 game_flag_byte_index(UINT16 flag_id) {
    return (UINT8)(flag_id >> 3u);
}

static UINT8 game_flag_mask(UINT16 flag_id) {
    return (UINT8)(1u << (flag_id & 7u));
}

void game_flags_clear_all(void) BANKED {
    UINT8 i;

    for (i = 0u; i < GAME_FLAG_BYTES; i++) {
        g_game_flags[i] = 0u;
    }
}

void game_flag_set(UINT16 flag_id) BANKED {
    UINT8 bi;

    if (flag_id >= GAME_FLAG_COUNT) return;

    bi = game_flag_byte_index(flag_id);
    g_game_flags[bi] |= game_flag_mask(flag_id);
}

void game_flag_clear(UINT16 flag_id) BANKED {
    UINT8 bi;

    if (flag_id >= GAME_FLAG_COUNT) return;

    bi = game_flag_byte_index(flag_id);
    g_game_flags[bi] &= (UINT8)(~game_flag_mask(flag_id));
}

UINT8 game_flag_get(UINT16 flag_id) BANKED {
    UINT8 bi;

    if (flag_id >= GAME_FLAG_COUNT) return 0u;

    bi = game_flag_byte_index(flag_id);
    return (g_game_flags[bi] & game_flag_mask(flag_id)) ? 1u : 0u;
}

void game_flags_copy_to(UINT8 *dst) BANKED {
    UINT8 i;

    if (dst == 0) return;

    for (i = 0u; i < GAME_FLAG_BYTES; i++) {
        dst[i] = g_game_flags[i];
    }
}

void game_flags_copy_from(const UINT8 *src) BANKED {
    UINT8 i;

    if (src == 0) return;

    for (i = 0u; i < GAME_FLAG_BYTES; i++) {
        g_game_flags[i] = src[i];
    }
}
