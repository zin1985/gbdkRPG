#pragma bank 2

#include <gb/gb.h>
#include "messages.h"

/*
 * rpg055 banked message data.
 *
 * This file is deliberately placed in ROM bank 2.
 * Keep message strings out of main.c to avoid bank 1 boundary pressure.
 */
BANKREF(messages_bank)

const char msg_common_npc_bank[] =
    "Aで はなしてね。\n"
    "まものの そうだんを\n"
    "しよう。";

const char msg_test_short_bank[] =
    "みちを\n"
    "みてね。";

const char * const message_table[MSG_COUNT] = {
    msg_common_npc_bank,
    msg_test_short_bank
};
