#pragma bank 17

#include <gb/gb.h>
#include "map_event_runtime.h"
#include "field_feature_runtime.h"

BANKREF(map_event_runtime_bank)

/*
 * rpg204:
 * Move the large map-event transition dispatcher out of Bank 0.
 * The actual transition helpers remain in main.c because they touch the
 * map/camera/player statics.  This banked dispatcher decides which helper to
 * call and returns whether it handled the event.
 */
void enter_town_marker(void);
void enter_port_marker(void);
void enter_dungeon_marker(void);
void enter_ruins_marker(void);
void enter_itil_tower_marker(void);
void advance_itil_tower_marker(void);
void leave_town_marker(void);
void leave_port_marker(void);
void leave_dungeon_marker(void);
void leave_itil_tower_marker(void);
void leave_ruins_marker(void);

UINT8 map_event_runtime_apply_transition(UINT8 event_id, UINT8 current_area) BANKED {
    switch (event_id) {
        case MAP_EVENT_TOWN:
            enter_town_marker();
            return 1u;
        case MAP_EVENT_PORT_TOWN:
            enter_port_marker();
            return 1u;
        case MAP_EVENT_DUNGEON:
            enter_dungeon_marker();
            return 1u;
        case MAP_EVENT_RUINS:
            enter_ruins_marker();
            return 1u;
        case MAP_EVENT_ITIL_TOWER:
            enter_itil_tower_marker();
            return 1u;
        case MAP_EVENT_ITIL_TOWER_NEXT:
            advance_itil_tower_marker();
            return 1u;
        case MAP_EVENT_FIELD_EXIT:
            leave_town_marker();
            return 1u;
        case MAP_EVENT_PORT_EXIT:
            leave_port_marker();
            return 1u;
        case MAP_EVENT_DUNGEON_EXIT:
            if (current_area == AREA_TOWER) leave_itil_tower_marker();
            else leave_dungeon_marker();
            return 1u;
        case MAP_EVENT_RUINS_EXIT:
            leave_ruins_marker();
            return 1u;
        default:
            return 0u;
    }
}
