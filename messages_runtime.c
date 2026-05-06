#include <gb/gb.h>
#include "dialogue.h"
#include "messages.h"

BANKREF_EXTERN(messages_bank)

extern const char * const message_table[MSG_COUNT];

/*
 * WRAM copy buffer.
 * dialogue_message() renders from this buffer after the source bank is restored,
 * so it never reads from a switched-out ROM bank.
 */
static char message_buffer[96];

static void message_copy_from_bank(UINT8 id) NONBANKED {
    UINT8 i;
    UINT8 save_bank;
    const char *src;

    if (id >= MSG_COUNT) {
        id = MSG_COMMON_NPC;
    }

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(messages_bank));

    src = message_table[id];

    for (i = 0u; i < 95u; i++) {
        message_buffer[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    message_buffer[95u] = '\0';

    SWITCH_ROM(save_bank);
}

void message_show(UINT8 id) NONBANKED {
    if (id == MSG_NONE) {
        return;
    }

    message_copy_from_bank(id);
    dialogue_message(message_buffer);
}

const char *message_get_buffered(UINT8 id) NONBANKED {
    if (id == MSG_NONE) {
        message_buffer[0] = '\0';
        return message_buffer;
    }
    message_copy_from_bank(id);
    return message_buffer;
}
