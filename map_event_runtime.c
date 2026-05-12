#pragma bank 17

#include <gb/gb.h>
#include "map_event_runtime.h"
#include "field_feature_runtime.h"
#include "messages.h"
#include "audio.h"
#include "quest.h"

BANKREF(map_event_runtime_bank)

/*
 * rpg221:
 * Normal area transitions are now data-driven in Bank17.
 * This avoids adding one Bank0 wrapper for every future town/dungeon/field.
 *
 * Rule for future expansion:
 * - Add ordinary town/field/dungeon transitions to area_transition_defs[] here.
 * - Do not add new enter_xxx_marker()/leave_xxx_marker() wrappers to main.c.
 * - Keep main.c responsible only for the generic change_area_marker() and
 *   special cases that must redraw the current field state.
 */

#define TRANS_AREA_ANY 0xFFu
#define QUEST_OP_NONE  0u
#define QUEST_OP_START 1u
#define QUEST_OP_ADV   2u

#define DIR_DOWN_U  0u
#define DIR_UP_U    1u
#define DIR_LEFT_U  2u
#define DIR_RIGHT_U 3u

typedef struct AreaTransitionDef {
    UINT8 event_id;
    UINT8 from_area;
    UINT8 before_msg;
    UINT8 to_area;
    UINT8 music;
    UINT8 tx;
    UINT8 ty;
    UINT8 dir;
    UINT8 after_msg;
    UINT8 quest_op;
    UINT8 quest_id;
} AreaTransitionDef;

void change_area_marker(UINT8 before_msg, UINT8 area, UINT8 music, UINT8 tx, UINT8 ty, UINT8 dir, UINT8 after_msg);

void enter_itil_tower_marker(void);
void advance_itil_tower_marker(void);
void leave_itil_tower_marker(void);

static const AreaTransitionDef area_transition_defs[] = {
    { MAP_EVENT_TOWN,         TRANS_AREA_ANY, MSG_ENTER_TOWN,    AREA_TOWN,    AUDIO_TRACK_TOWN,    2u, 13u, DIR_DOWN_U,  MSG_ARRIVE_TOWN,    QUEST_OP_NONE,  0u },
    { MAP_EVENT_FIELD_EXIT,   AREA_TOWN,      MSG_BACK_FIELD,    AREA_FIELD,   AUDIO_TRACK_FIELD,  13u,  2u, DIR_RIGHT_U, MSG_LEFT_FIELD,     QUEST_OP_NONE,  0u },

    { MAP_EVENT_PORT_TOWN,    TRANS_AREA_ANY, MSG_ENTER_PORT,    AREA_PORT,    AUDIO_TRACK_TOWN,    2u, 13u, DIR_DOWN_U,  MSG_ARRIVE_PORT,    QUEST_OP_NONE,  0u },
    { MAP_EVENT_PORT_EXIT,    AREA_PORT,      MSG_BACK_FIELD,    AREA_FIELD,   AUDIO_TRACK_FIELD,  13u, 14u, DIR_RIGHT_U, MSG_LEFT_FIELD,     QUEST_OP_NONE,  0u },

    { MAP_EVENT_DUNGEON,      TRANS_AREA_ANY, MSG_ENTER_DUNGEON, AREA_DUNGEON, AUDIO_TRACK_DUNGEON, 2u, 13u, DIR_DOWN_U,  MSG_ARRIVE_DUNGEON, QUEST_OP_START, QUEST_LOST_KEY },
    { MAP_EVENT_DUNGEON_EXIT, AREA_DUNGEON,   MSG_DUNGEON_EXIT,  AREA_FIELD,   AUDIO_TRACK_FIELD,   2u, 14u, DIR_RIGHT_U, MSG_NONE,           QUEST_OP_NONE,  0u },

    { MAP_EVENT_RUINS,        TRANS_AREA_ANY, MSG_ENTER_RUINS,   AREA_RUINS,   AUDIO_TRACK_RUINS,  13u, 13u, DIR_UP_U,    MSG_ARRIVE_RUINS,   QUEST_OP_ADV,   QUEST_LOST_KEY },
    { MAP_EVENT_RUINS_EXIT,   AREA_RUINS,     MSG_DUNGEON_EXIT,  AREA_FIELD,   AUDIO_TRACK_FIELD,   2u,  2u, DIR_RIGHT_U, MSG_NONE,           QUEST_OP_NONE,  0u }
};

static void area_transition_apply_quest(UINT8 op, UINT8 quest_id) BANKED {
    if (op == QUEST_OP_START) {
        quest_start(quest_id);
    } else if (op == QUEST_OP_ADV) {
        quest_advance(quest_id);
    }
}

static UINT8 area_transition_try_table(UINT8 event_id, UINT8 current_area) BANKED {
    UINT8 i;
    const AreaTransitionDef *def;

    for (i = 0u; i < (UINT8)(sizeof(area_transition_defs) / sizeof(area_transition_defs[0])); i++) {
        def = &area_transition_defs[i];
        if (def->event_id != event_id) continue;
        if (def->from_area != TRANS_AREA_ANY && def->from_area != current_area) continue;

        change_area_marker(def->before_msg, def->to_area, def->music, def->tx, def->ty, def->dir, def->after_msg);
        area_transition_apply_quest(def->quest_op, def->quest_id);
        return 1u;
    }
    return 0u;
}

UINT8 map_event_runtime_apply_transition(UINT8 event_id, UINT8 current_area) BANKED {
    if (event_id == MAP_EVENT_ITIL_TOWER) {
        enter_itil_tower_marker();
        return 1u;
    }

    if (event_id == MAP_EVENT_ITIL_TOWER_NEXT) {
        advance_itil_tower_marker();
        return 1u;
    }

    if (event_id == MAP_EVENT_DUNGEON_EXIT && current_area == AREA_TOWER) {
        leave_itil_tower_marker();
        return 1u;
    }

    return area_transition_try_table(event_id, current_area);
}
