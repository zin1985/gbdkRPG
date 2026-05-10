#pragma bank 10

#include <gb/gb.h>
#include "quest.h"


static UINT8 g_quest_states[QUEST_MAX];

void quest_init(void) BANKED {
    UINT8 i;

    for (i = 0u; i < QUEST_MAX; i++) {
        g_quest_states[i] = QST_NONE;
    }
}

UINT8 quest_get_state(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return QST_NONE;
    return g_quest_states[quest_id];
}

void quest_set_state(UINT8 quest_id, UINT8 state) BANKED {
    if (quest_id >= QUEST_MAX) return;
    if (state > QST_DONE) state = QST_DONE;

    g_quest_states[quest_id] = state;
}

void quest_start(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return;

    if (g_quest_states[quest_id] == QST_NONE) {
        g_quest_states[quest_id] = QST_START;
    }
}

void quest_advance(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return;

    if (g_quest_states[quest_id] == QST_NONE) {
        g_quest_states[quest_id] = QST_START;
    } else if (g_quest_states[quest_id] < QST_READY) {
        g_quest_states[quest_id]++;
    }
}

void quest_ready(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return;
    if (g_quest_states[quest_id] != QST_DONE) {
        g_quest_states[quest_id] = QST_READY;
    }
}

void quest_complete(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return;
    g_quest_states[quest_id] = QST_DONE;
}

UINT8 quest_is_done(UINT8 quest_id) BANKED {
    if (quest_id >= QUEST_MAX) return 0u;
    return (g_quest_states[quest_id] == QST_DONE) ? 1u : 0u;
}

void quest_copy_to(UINT8 *dst) BANKED {
    UINT8 i;

    if (dst == 0) return;

    for (i = 0u; i < QUEST_MAX; i++) {
        dst[i] = g_quest_states[i];
    }
}

void quest_copy_from(const UINT8 *src) BANKED {
    UINT8 i;

    if (src == 0) return;

    for (i = 0u; i < QUEST_MAX; i++) {
        if (src[i] <= QST_DONE) {
            g_quest_states[i] = src[i];
        } else {
            g_quest_states[i] = QST_NONE;
        }
    }
}
