#ifndef MESSAGES_H
#define MESSAGES_H

#include <gb/gb.h>

/*
 * rpg055:
 * Actor/dialogue code should use small message IDs in main.c.
 * Actual UTF-8 message strings live in messages_bank.c.
 */
#define MSG_COMMON_NPC  0u
#define MSG_TEST_SHORT  1u
#define MSG_NONE        0xFFu
#define MSG_COUNT       2u

void message_show(UINT8 id) NONBANKED;

#endif
