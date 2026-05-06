#ifndef BATTLE_TEXT_H
#define BATTLE_TEXT_H

#include <gb/gb.h>

BANKREF_EXTERN(battle_text_bank)

const char *battle_compact_message(const char *text) BANKED;
const char *battle_format_damage_message(const char *prefix, UINT16 value) BANKED;

#endif
