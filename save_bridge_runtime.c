#pragma bank 5

#include <gb/gb.h>
#include "save_bridge_runtime.h"
#include "inventory.h"
#include "party_runtime.h"
#include "actor_runtime.h"
#include "shop_runtime.h"
#include "field_feature_runtime.h"

BANKREF(save_bridge_runtime_bank)

#define SAVE_BRIDGE_MAP16_W 16u
#define SAVE_BRIDGE_MAP16_H 16u
#define SAVE_BRIDGE_TILE16_PX 16u

extern UINT8 event_flags[];
extern UINT8 current_area;
extern UINT8 player_tx;
extern UINT8 player_ty;
extern UINT8 spawn_player_tx;
extern UINT8 spawn_player_ty;
extern UINT8 player_px;
extern UINT8 player_py;
extern UINT8 player_target_tx;
extern UINT8 player_target_ty;
extern UINT8 player_target_px;
extern UINT8 player_target_py;
extern UINT8 battle_enemy_rank;
extern UINT8 player_moving;
extern UINT8 move_pixels_remaining;
extern UINT16 map_idle_frames;
extern UINT8 map_overlay_visible;
extern UINT8 player_dir;

void snap_camera_to_player(void);

void save_bridge_build_snapshot(SaveSnapshot *out) BANKED {
    UINT8 i;

    if (out == 0) return;

    out->area = current_area;
    out->player_tx = player_tx;
    out->player_ty = player_ty;
    out->player_dir = (UINT8)(player_dir & 3u);
    out->battle_enemy_rank = battle_enemy_rank;
    out->pot_opened = shop_runtime_get_pot_opened();
    out->money = inventory_get_money();

    for (i = 0u; i < SAVE_EVENT_FLAG_COUNT; i++) {
        out->event_flags[i] = event_flags[i];
    }

    inventory_copy_to(out->inventory_counts);
    party_save_copy_to(&out->party);
}

void save_bridge_apply_snapshot(const SaveSnapshot *in) BANKED {
    UINT8 i;

    if (in == 0) return;

    current_area = in->area;
    if (current_area > AREA_TOWER) current_area = AREA_FIELD;

    player_tx = in->player_tx;
    player_ty = in->player_ty;
    if (player_tx >= SAVE_BRIDGE_MAP16_W) player_tx = 1u;
    if (player_ty >= SAVE_BRIDGE_MAP16_H) player_ty = 2u;
    spawn_player_tx = player_tx;
    spawn_player_ty = player_ty;
    player_px = (UINT8)(player_tx * SAVE_BRIDGE_TILE16_PX);
    player_py = (UINT8)(player_ty * SAVE_BRIDGE_TILE16_PX);
    player_target_tx = player_tx;
    player_target_ty = player_ty;
    player_target_px = player_px;
    player_target_py = player_py;
    player_dir = (UINT8)(in->player_dir & 3u);
    battle_enemy_rank = in->battle_enemy_rank;
    if (battle_enemy_rank == 0u) battle_enemy_rank = 1u;

    for (i = 0u; i < SAVE_EVENT_FLAG_COUNT; i++) {
        event_flags[i] = in->event_flags[i];
    }

    inventory_copy_from(in->inventory_counts);
    inventory_set_money(in->money);
    party_save_copy_from(&in->party);
    shop_runtime_set_pot_opened(in->pot_opened);

    actor_runtime_apply_area_npcs(current_area);
    player_moving = 0u;
    move_pixels_remaining = 0u;
    map_idle_frames = 0u;
    map_overlay_visible = 0u;
    snap_camera_to_player();
}
