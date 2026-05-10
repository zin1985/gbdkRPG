#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <gb/gb.h>

BANKREF_EXTERN(dialogue_bank)

void dialogue_init(void) BANKED;
void dialogue_hide(void) BANKED;
UINT8 dialogue_is_active(void) BANKED;
void dialogue_message(const char *text) BANKED;
void dialogue_message_nowait_window(const char *text) BANKED;

#endif
