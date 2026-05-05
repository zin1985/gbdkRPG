#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <gb/gb.h>

void dialogue_init(void);
void dialogue_hide(void);
UINT8 dialogue_is_active(void);
void dialogue_message(const char *text);
void dialogue_message_nowait_window(const char *text);

#endif
