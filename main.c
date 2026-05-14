
#include <gb/gb.h>
#include "audio.h"
#include "dialogue.h"
#include "messages.h"
#include "jpfont.h"
#include "sprites.h"
#include "banked_graphics.h"
#include "map_data.h"
#include "battle_data.h"
#include "battle_text.h"
#include "party_runtime.h"
#include "inventory.h"
#include "menu_runtime.h"
#include "actor_runtime.h"
#include "quest.h"
#include "field_feature_runtime.h"
#include "battle_skill_runtime.h"
#include "skill_defs.h"
#include "battle_growth_runtime.h"
#include "field_overlay_runtime.h"
#include "shop_runtime.h"
#include "save_runtime.h"
#include "save_bridge_runtime.h"
#include "revive_runtime.h"
#include "itil_tower_runtime.h"
#include "map_event_runtime.h"
#include "field_map_render_runtime.h"
#include "battle_reward_runtime.h"
#include "battle_command_ui_runtime.h"
#include "battle_status_ui_runtime.h"
#include "cgb_fx_runtime.h"

BANKREF_EXTERN(sprite_data_bank)

#define FIELD_MAP16_W 16u
#define FIELD_MAP16_H 16u
#define TOWN_MAP16_W 16u
#define TOWN_MAP16_H 16u
#define MAP16_W FIELD_MAP16_W
#define MAP16_H FIELD_MAP16_H
#define TILE16_PX 16u
#define BG_W 20u
#define BG_H 18u

#define BG_DRAW_W 32u
#define BG_DRAW_H 32u
#define DRAW_MAP16_W MAP16_W
#define DRAW_MAP16_H MAP16_H
#define SCREEN_PX_W 160u
#define SCREEN_PX_H 144u

/* rpg233: keep the field player away from OAM 0-15.
 * Battle enemies and several legacy UI paths aggressively reuse/hide OAM 0-23.
 * On field return this could leave only one quarter of the player visible on
 * some emulators/flash carts.  Use a later, field-only OAM slot and reset the
 * four player OBJ attributes every frame.
 */
#define PLAYER_SPRITE_BASE 16u
#define NPC0_SPRITE_BASE   4u
#define ENEMY0_SPRITE_BASE 8u

#define TEST_ACTOR_SPRITE_BASE 12u
#define BATTLE_PARTY_ICON0_SPRITE 24u
#define BATTLE_PARTY_ICON1_SPRITE 26u
#define BATTLE_PARTY_ICON2_SPRITE 28u
#define BATTLE_PARTY_ICON_TILE_BASE NPC0_TILE_BASE

#define PLAYER_TILE_BASE 0u
#define NPC0_TILE_BASE   32u
#define ENEMY0_TILE_BASE 64u
#define MAP_TILE_BASE    96u
#define MAP_TILE_FLOOR   (MAP_TILE_BASE + 0u)
#define MAP_TILE_WALL_TL (MAP_TILE_BASE + 1u)
#define MAP_TILE_WALL_TR (MAP_TILE_BASE + 2u)
#define MAP_TILE_WALL_BL (MAP_TILE_BASE + 3u)
#define MAP_TILE_WALL_BR (MAP_TILE_BASE + 4u)
#define MAP_TILE_CAP_TL  (MAP_TILE_BASE + 5u)
#define MAP_TILE_CAP_TR  (MAP_TILE_BASE + 6u)
#define MAP_TILE_CAP_BL  (MAP_TILE_BASE + 7u)
#define MAP_TILE_CAP_BR  (MAP_TILE_BASE + 8u)
#define MAP_TILE_TOWN_TL (MAP_TILE_BASE + 9u)
#define MAP_TILE_TOWN_TR (MAP_TILE_BASE + 10u)
#define MAP_TILE_TOWN_BL (MAP_TILE_BASE + 11u)
#define MAP_TILE_TOWN_BR (MAP_TILE_BASE + 12u)
#define MAP_TILE_FOREST_TL (MAP_TILE_BASE + 13u)
#define MAP_TILE_FOREST_TR (MAP_TILE_BASE + 14u)
#define MAP_TILE_FOREST_BL (MAP_TILE_BASE + 15u)
#define MAP_TILE_FOREST_BR (MAP_TILE_BASE + 16u)
#define MAP_TILE_DUNGEON_WALL_TL (MAP_TILE_BASE + 17u)
#define MAP_TILE_DUNGEON_WALL_TR (MAP_TILE_BASE + 18u)
#define MAP_TILE_DUNGEON_WALL_BL (MAP_TILE_BASE + 19u)
#define MAP_TILE_DUNGEON_WALL_BR (MAP_TILE_BASE + 20u)
#define MAP_TILE_DUNGEON_PIT_TL  (MAP_TILE_BASE + 21u)
#define MAP_TILE_DUNGEON_PIT_TR  (MAP_TILE_BASE + 22u)
#define MAP_TILE_DUNGEON_PIT_BL  (MAP_TILE_BASE + 23u)
#define MAP_TILE_DUNGEON_PIT_BR  (MAP_TILE_BASE + 24u)
#define MAP_TILE_CHEST_TL (MAP_TILE_BASE + 25u)
#define MAP_TILE_CHEST_TR (MAP_TILE_BASE + 26u)
#define MAP_TILE_CHEST_BL (MAP_TILE_BASE + 27u)
#define MAP_TILE_CHEST_BR (MAP_TILE_BASE + 28u)
#define FOREST_BG_X 6u
#define FOREST_BG_Y 5u

#define PLAYER_STEP_PIXELS 16u
#define WALK_ANIM_FRAMES 12u
#define ENEMY_ACTOR_INDEX 1u
#define CAMERA_CENTER_X 72u
#define CAMERA_CENTER_Y 64u
#define CAMERA_SPEED_PX 2u
#define RANDOM_ENCOUNTER_RATE 20u
#define RANDOM_ENCOUNTER_GRACE_STEPS 3u

#define BATTLE_MAX_ENEMY_COUNT BATTLE_DATA_MAX_ENEMIES
#define BATTLE_ENEMY0_SPRITE_BASE 0u
#define BATTLE_ENEMY1_SPRITE_BASE 8u
#define BATTLE_ENEMY2_SPRITE_BASE 16u

#define BATTLE_ENEMY_SPRITE_Y 40u
#define BATTLE_CURSOR_SPRITE 30u
#define BATTLE_CURSOR_TILE 44u
#define BATTLE_ENEMY_TILE_BASE 80u
#define BATTLE_ENEMY_TILE_COUNT 48u
#define BATTLE_MSG_X 0u
#define BATTLE_MSG_Y 13u
#define BATTLE_MSG_W 20u
#define BATTLE_MSG_H 5u

#define BATTLE_BG_SHIFT_X 0u
#define BATTLE_BG_SCROLL_X 0u
#define BATTLE_BG_X(x) ((UINT8)(x))
#define draw_bkg_box(x0, y0, w, h) jp_draw_bkg_frame(BATTLE_BG_X(x0), (y0), (w), (h))
#define battle_bkg_clear_area(x0, y0, w, h) jp_bkg_clear_area(BATTLE_BG_X(x0), (y0), (w), (h))
#define battle_put_bkg_text(col, row, text) jp_put_bkg_text(BATTLE_BG_X(col), (row), (text))
#define BATTLE_BG_Y(y) ((UINT8)(y))

#define BATTLE_DIRTY_NONE      0x00u
#define BATTLE_DIRTY_PARTY_HP  0x01u
#define BATTLE_DIRTY_PARTY_MP  0x02u
#define BATTLE_DIRTY_MESSAGE   0x04u
#define BATTLE_DIRTY_CURSOR    0x08u
#define BATTLE_DIRTY_ENEMY_OAM 0x10u

#define EVENT_FLAG_COUNT 32u
#define FLAG_ENEMY_DEFEATED 0u

#define GROWTH_NONE 0u
#define GROWTH_HP 1u
#define GROWTH_MP 2u
#define GROWTH_ATK 3u
#define GROWTH_DEF 4u
#define GROWTH_SKILL 5u
#define GROWTH_HEAL 6u
#define GROWTH_AGI 7u
#define GROWTH_STAT_COUNT 7u

#define PLAYER_HP_MP_MAX 999u
#define PLAYER_STAT_MAX 255u
#define ENEMY_RANK_MAX 5u

typedef struct GrowthResult {
    UINT8 actor_id;
    UINT8 stat_type;
    UINT16 old_value;
    UINT16 new_value;
} GrowthResult;

#define MENU_STATUS 0u
#define MENU_ITEM 1u
#define MENU_EQUIP 2u
#define MENU_OBJECTIVE 3u
#define MENU_COUNT 4u

/* rpg232: keep the per-direction/per-walk-frame actor sheet intact.
 * rpg231 copied the first 16x16 frame into every direction slot, so the
 * player always looked like the same sideways/right-facing sprite.
 * This flag now means "load each 4-tile frame separately", not duplicate it.
 */
#define SAFE_PER_FRAME_ACTOR_LOAD 1

typedef enum GameMode {
    MODE_MAP = 0,
    MODE_BATTLE
} GameMode;

typedef enum BattleState {
    BSTATE_PLAYER = 0,
    BSTATE_SKILL,
    BSTATE_TARGET,
    BSTATE_ALLY_TARGET,
    BSTATE_ENEMY,
    BSTATE_WIN,
    BSTATE_LOSE,
    BSTATE_ESCAPE
} BattleState;

typedef enum CommandType {
    CMD_ATTACK = 0,
    CMD_MAGIC,
    CMD_SKILL,
    CMD_ITEM,
    CMD_DEFEND,
    CMD_RUN,
    CMD_COUNT
} CommandType;

typedef enum Direction {
    DIR_DOWN = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef enum ActorKind {
    ACTOR_KIND_NPC = 0,
    ACTOR_KIND_ENEMY
} ActorKind;

typedef struct Fighter {
    const char *name;
    UINT16 max_hp;
    UINT16 hp;
    UINT16 max_mp;
    UINT16 mp;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill_power;
    UINT8 heal_power;
    UINT8 magic_mastery;
    UINT8 agility;
} Fighter;

typedef struct Actor {
    ActorKind kind;
    UINT8 active;
    UINT8 solid;
    UINT8 talkable;
    UINT8 tx;
    UINT8 ty;
    Direction dir;
    UINT8 walk_frame;
    UINT8 sprite_base;
    UINT8 tile_base;
    UINT8 message_id;
    Fighter stats;
} Actor;

Actor actors[3u];
#define ACTOR_COUNT ((UINT8)(sizeof(actors) / sizeof(actors[0])))

static GameMode game_mode;
static BattleState battle_state;
static UINT8 menu_index;
static UINT8 rand_seed;
static UINT8 encounter_grace_steps;
UINT8 event_flags[EVENT_FLAG_COUNT];
UINT8 last_growth_type;
GrowthResult last_growth;
UINT8 battle_enemy_rank;
static UINT8 cave_boss_battle_pending;
static SaveSnapshot save_snapshot_work;

static UINT8 prev_keys;
static UINT8 current_enemy_index;
UINT8 current_area;
static UINT8 camera_tx;
static UINT8 camera_ty;
static UINT8 camera_px;
static UINT8 camera_py;
static UINT8 camera_target_px;
static UINT8 camera_target_py;
static Fighter player_battle;
static Fighter enemy_battles[BATTLE_MAX_ENEMY_COUNT];
static BattleEnemyData battle_enemy_data_slots[BATTLE_MAX_ENEMY_COUNT];
static UINT8 battle_enemy_sprite_kinds[BATTLE_MAX_ENEMY_COUNT];
static UINT8 battle_enemy_size_kinds[BATTLE_MAX_ENEMY_COUNT];
static UINT8 battle_enemy_count;
static UINT8 battle_target_index;
static UINT8 battle_pending_action;
static UINT8 battle_dirty_flags;
static const char *battle_message_text;
static UINT8 battle_first_player_refresh_pending;
static UINT8 battle_screen_ready;
static UINT8 battle_party_turn_slot;
static UINT8 battle_selected_skill;
static UINT8 battle_guard_flags;
UINT16 map_idle_frames;
UINT8 map_overlay_visible;
#define enemy_battle (enemy_battles[battle_target_index])
UINT16 player_max_hp_stat;
UINT16 player_max_mp_stat;
UINT8 player_attack_stat;
UINT8 player_defense_stat;
UINT8 player_skill_power_stat;
UINT8 player_heal_power_stat;
UINT8 player_agility_stat;
UINT8 player_tx;
UINT8 player_ty;
UINT8 spawn_player_tx;
UINT8 spawn_player_ty;
UINT8 player_px;
UINT8 player_py;
UINT8 player_target_tx;
UINT8 player_target_ty;
UINT8 player_target_px;
UINT8 player_target_py;
UINT8 player_dir;
static UINT8 walk_frame;
UINT8 player_moving;
static INT8 player_vx;
static INT8 player_vy;
UINT8 move_pixels_remaining;
static UINT8 walk_anim_counter;

UINT8 random_u8(void);
static void init_event_flags(void);
static void set_event_flag(UINT8 id);
static UINT8 check_event_flag(UINT8 id);
static UINT8 actor_px(UINT8 tx);
static UINT8 actor_py(UINT8 ty);
static INT16 screen_x_from_world(UINT8 px);
static INT16 screen_y_from_world(UINT8 py);
static UINT8 camera_tile_from_px(UINT8 px);
static UINT8 step_toward_u8(UINT8 current, UINT8 target, UINT8 step);
static void update_camera_target_for_player(void);
static void apply_camera_scroll(void);
void snap_camera_to_player(void);
static UINT8 update_camera_for_player(void);
static void revive_enemy_actor(void);
static void prompt_enemy_revive_choice(void);
static void open_main_menu(void);
static UINT8 current_collision16_at(UINT8 tx, UINT8 ty);
static UINT8 current_object16_at(UINT8 tx, UINT8 ty);
static void set_player_frame(Direction dir, UINT8 frame);
static void move_player_sprite_px(INT16 px, INT16 py);
static void set_actor_frame(const Actor *actor);
static void move_actor_sprite(const Actor *actor);
static void hide_actor_sprite(const Actor *actor);
static UINT8 actor_visible_in_current_area(const Actor *actor);
static void draw_all_actors(void);
static void load_actor_sprite_data_safe(UINT8 tile_base, const unsigned char *tiles);
static void init_map_sprites(void);
static void restore_field_vram_state(void);
static void load_map_mode(void);
static UINT8 map16_is_blocked(UINT8 tx, UINT8 ty);
static UINT8 actor_at_tile(UINT8 tx, UINT8 ty, UINT8 *enemy_index);
static Direction opposite_dir(Direction dir);
static INT8 find_talkable_actor(void);
#define map_event_at_tile(tx, ty) field_feature_map_event(current_area, (tx), (ty))
#define CAMERA_MAX_PX_X ((UINT8)((MAP16_W * TILE16_PX > SCREEN_PX_W) ? (MAP16_W * TILE16_PX - SCREEN_PX_W) : 0u))
#define CAMERA_MAX_PX_Y ((UINT8)((MAP16_H * TILE16_PX > SCREEN_PX_H) ? (MAP16_H * TILE16_PX - SCREEN_PX_H) : 0u))
static void warp_player_to_tile(UINT8 tx, UINT8 ty, Direction dir);
static void check_step_event(void);
static void inspect_map_event(UINT8 event_id);
static void try_interact(void);

static void start_move(Direction dir);
static void update_player_movement(void);
static void map_input(void);
static UINT16 calc_attack_damage(const Fighter *attacker, const Fighter *defender);
static void draw_battle_enemy_names(void);
static void draw_battle_message_area(void);
static void battle_prepare_first_command_ui(void);
static void battle_enter_render_once(void);
static void battle_update_dirty(void);
static void battle_set_message_dirty(const char *text);
void battle_show_message(const char *text);
void battle_show_damage_message(const char *prefix, UINT16 value);
static void battle_move_command_cursor_obj(void);
static void battle_move_target_cursor_obj(void);
static void battle_hide_command_cursor_obj(void);
static void battle_start_target_select(UINT8 action_id);
static UINT8 battle_select_next_alive_target(UINT8 forward);
static void battle_target_input(void);
static void battle_flash_enemy_sprite(UINT8 enemy_index);
static void battle_refresh_enemy_sprites_compact(UINT8 hide_first);


static void battle_reset_bg_origin(void);
static void battle_hide_window_and_reset_scroll(void);
static void hide_all_sprites_safe(void);
static void battle_copy_enemy_from_data(UINT8 slot);
static UINT8 battle_select_first_alive(void);
static UINT8 battle_ensure_selected_alive(void);
static void draw_battle_menu(void);
static void update_battle_menu_cursor(UINT8 old_index, UINT8 new_index);
static void update_battle_status(void);
static void init_battle_from_enemy(UINT8 enemy_index);
static UINT8 field_random_encounter_should_start(void);
static void battle_start_effect(void);
static void enter_battle_screen(void);
static void init_random_battle_from_field(void);
static void battle_prepare_enemy_rank(void);
static void return_to_map_after_battle(UINT8 won);
static void battle_load_current_actor(Fighter *out);
static UINT8 battle_select_next_party_turn(void);
static void battle_finish_party_action(void);
static void battle_heal_actor_slot(UINT8 target_slot, UINT16 amount);
static void player_attack(void);
static void player_run(void);
static void battle_skill_input(void);
static void battle_ally_target_input(void);
static void field_overlay_hide(void);
static void field_overlay_show(void);
static void enemy_turn(void);
static void battle_input(void);
static void init_game(void);

UINT8 random_u8(void) { rand_seed = (UINT8)(rand_seed * 17u + 29u); return rand_seed; }

static UINT8 current_collision16_at(UINT8 tx, UINT8 ty) {
    if (tx >= MAP16_W || ty >= MAP16_H) return 1u;
    return map_data_collision16_at(current_area, tx, ty);
}

static UINT8 current_object16_at(UINT8 tx, UINT8 ty) {
    if (tx >= MAP16_W || ty >= MAP16_H) return 1u;
    return map_data_object16_at(current_area, tx, ty);
}

static void init_event_flags(void) {
    UINT8 i;
    for (i = 0u; i < EVENT_FLAG_COUNT; i++) event_flags[i] = 0u;
}

static void set_event_flag(UINT8 id) {
    if (id < EVENT_FLAG_COUNT) event_flags[id] = 1u;
}

static UINT8 check_event_flag(UINT8 id) {
    if (id < EVENT_FLAG_COUNT) return event_flags[id];
    return 0u;
}

static UINT8 actor_px(UINT8 tx) { return (UINT8)(tx * TILE16_PX); }
static UINT8 actor_py(UINT8 ty) { return (UINT8)(ty * TILE16_PX); }

static INT16 screen_x_from_world(UINT8 px) { return (INT16)((INT16)px - (INT16)camera_px); }
static INT16 screen_y_from_world(UINT8 py) { return (INT16)((INT16)py - (INT16)camera_py); }



static UINT8 camera_tile_from_px(UINT8 px) { return (UINT8)(px / TILE16_PX); }

static UINT8 step_toward_u8(UINT8 current, UINT8 target, UINT8 step) {
    UINT8 delta;
    if (current < target) {
        delta = (UINT8)(target - current);
        return (UINT8)(current + ((delta < step) ? delta : step));
    }
    if (current > target) {
        delta = (UINT8)(current - target);
        return (UINT8)(current - ((delta < step) ? delta : step));
    }
    return current;
}

static void update_camera_target_for_player(void) {
    UINT8 desired_x;
    UINT8 desired_y;

    desired_x = (player_px > CAMERA_CENTER_X) ? (UINT8)(player_px - CAMERA_CENTER_X) : 0u;
    desired_y = (player_py > CAMERA_CENTER_Y) ? (UINT8)(player_py - CAMERA_CENTER_Y) : 0u;

    if (desired_x > CAMERA_MAX_PX_X) desired_x = CAMERA_MAX_PX_X;
    if (desired_y > CAMERA_MAX_PX_Y) desired_y = CAMERA_MAX_PX_Y;

    camera_target_px = desired_x;
    camera_target_py = desired_y;
}

static void apply_camera_scroll(void) {
    
    move_bkg(camera_px, camera_py);
}

void snap_camera_to_player(void) {
    update_camera_target_for_player();
    camera_px = camera_target_px;
    camera_py = camera_target_py;
    camera_tx = camera_tile_from_px(camera_px);
    camera_ty = camera_tile_from_px(camera_py);
    apply_camera_scroll();
}

static UINT8 update_camera_for_player(void) {
    UINT8 old_px = camera_px;
    UINT8 old_py = camera_py;

    update_camera_target_for_player();
    camera_px = step_toward_u8(camera_px, camera_target_px, CAMERA_SPEED_PX);
    camera_py = step_toward_u8(camera_py, camera_target_py, CAMERA_SPEED_PX);

    
    camera_tx = camera_tile_from_px(camera_px);
    camera_ty = camera_tile_from_px(camera_py);
    apply_camera_scroll();

    return (UINT8)((old_px != camera_px) || (old_py != camera_py));
}

static void revive_enemy_actor(void) {
    actors[ENEMY_ACTOR_INDEX].active = 1u;
    actors[ENEMY_ACTOR_INDEX].solid = 1u;
    actors[ENEMY_ACTOR_INDEX].stats.hp = actors[ENEMY_ACTOR_INDEX].stats.max_hp;
    actors[ENEMY_ACTOR_INDEX].dir = DIR_DOWN;
    actors[ENEMY_ACTOR_INDEX].walk_frame = 0u;
    event_flags[FLAG_ENEMY_DEFEATED] = 0u;
}

static void prompt_enemy_revive_choice(void) {
    if (actors[ENEMY_ACTOR_INDEX].active) {
        message_show(MSG_ENEMY_STILL);
        return;
    }

    dialogue_message_nowait_window(message_get_buffered(MSG_PROMPT_REVIVE));
    if (revive_runtime_wait_choice_ab()) {
        dialogue_hide();
        revive_enemy_actor();
        draw_all_actors();
        message_show(MSG_ENEMY_REVIVED);
    } else {
        dialogue_hide();
        message_show(MSG_LEAVE_AS_IS);
    }
    draw_all_actors();
}

static void open_main_menu(void) {
    UINT8 defeated;

    DISPLAY_OFF;
    HIDE_WIN;
    HIDE_SPRITES;
    SHOW_BKG;
    move_bkg(0u, 0u);
    audio_waitpadup();

    defeated = check_event_flag(FLAG_ENEMY_DEFEATED);
    menu_runtime_open(defeated);

    restore_field_vram_state();
    audio_play_music(field_feature_music_track(current_area));
}

static void set_player_frame(Direction dir, UINT8 frame) {
    UINT8 base;

    /* rpg233:
     * Make the field player renderer self-contained.
     * If battle left OBJ size/properties in a different state, the player
     * could be reduced to a single visible 8x8 quarter.  Force 8x8 mode and
     * clear priority/palette/flip flags for all four player OAM entries before
     * assigning the 2x2 frame tiles.
     */
    SPRITES_8x8;
    set_sprite_prop(PLAYER_SPRITE_BASE + 0u, 0u);
    set_sprite_prop(PLAYER_SPRITE_BASE + 1u, 0u);
    set_sprite_prop(PLAYER_SPRITE_BASE + 2u, 0u);
    set_sprite_prop(PLAYER_SPRITE_BASE + 3u, 0u);

    base = (UINT8)(((UINT8)dir * 2u + frame) * 4u);
    set_sprite_tile(PLAYER_SPRITE_BASE + 0u, PLAYER_TILE_BASE + base + 0u);
    set_sprite_tile(PLAYER_SPRITE_BASE + 1u, PLAYER_TILE_BASE + base + 1u);
    set_sprite_tile(PLAYER_SPRITE_BASE + 2u, PLAYER_TILE_BASE + base + 2u);
    set_sprite_tile(PLAYER_SPRITE_BASE + 3u, PLAYER_TILE_BASE + base + 3u);
}

static void move_player_sprite_px(INT16 px, INT16 py) {
    if (px < -16 || py < -16 || px > 160 || py > 144) {
        move_sprite(PLAYER_SPRITE_BASE + 0u, 0u, 0u);
        move_sprite(PLAYER_SPRITE_BASE + 1u, 0u, 0u);
        move_sprite(PLAYER_SPRITE_BASE + 2u, 0u, 0u);
        move_sprite(PLAYER_SPRITE_BASE + 3u, 0u, 0u);
        return;
    }
    move_sprite(PLAYER_SPRITE_BASE + 0u, (UINT8)(px + 8),  (UINT8)(py + 16));
    move_sprite(PLAYER_SPRITE_BASE + 1u, (UINT8)(px + 16), (UINT8)(py + 16));
    move_sprite(PLAYER_SPRITE_BASE + 2u, (UINT8)(px + 8),  (UINT8)(py + 24));
    move_sprite(PLAYER_SPRITE_BASE + 3u, (UINT8)(px + 16), (UINT8)(py + 24));
}

static void set_actor_frame(const Actor *actor) {
    UINT8 base;
    base = (UINT8)(actor->tile_base + (((UINT8)actor->dir * 2u + actor->walk_frame) * 4u));
    set_sprite_tile(actor->sprite_base + 0u, base + 0u);
    set_sprite_tile(actor->sprite_base + 1u, base + 1u);
    set_sprite_tile(actor->sprite_base + 2u, base + 2u);
    set_sprite_tile(actor->sprite_base + 3u, base + 3u);
}

static void move_actor_sprite(const Actor *actor) {
    INT16 x;
    INT16 y;
    x = screen_x_from_world(actor_px(actor->tx));
    y = screen_y_from_world(actor_py(actor->ty));
    if (x < -16 || y < -16 || x > 160 || y > 144) {
        hide_actor_sprite(actor);
        return;
    }
    move_sprite(actor->sprite_base + 0u, (UINT8)(x + 8),  (UINT8)(y + 16));
    move_sprite(actor->sprite_base + 1u, (UINT8)(x + 16), (UINT8)(y + 16));
    move_sprite(actor->sprite_base + 2u, (UINT8)(x + 8),  (UINT8)(y + 24));
    move_sprite(actor->sprite_base + 3u, (UINT8)(x + 16), (UINT8)(y + 24));
}

static void hide_actor_sprite(const Actor *actor) {
    move_sprite(actor->sprite_base + 0u, 0u, 0u);
    move_sprite(actor->sprite_base + 1u, 0u, 0u);
    move_sprite(actor->sprite_base + 2u, 0u, 0u);
    move_sprite(actor->sprite_base + 3u, 0u, 0u);
}

static UINT8 actor_visible_in_current_area(const Actor *actor) {
    if (current_area == AREA_FIELD) {
        
        return 0u;
    }

    if (current_area == AREA_TOWN || current_area == AREA_PORT || current_area == AREA_DUNGEON || current_area == AREA_RUINS || current_area == AREA_TOWER) {
        return (UINT8)(actor->kind == ACTOR_KIND_NPC);
    }

    return 0u;
}

static void draw_all_actors(void) {
    UINT8 i;
    set_player_frame(player_dir, walk_frame);
    move_player_sprite_px(screen_x_from_world(player_px), screen_y_from_world(player_py));
    for (i = 0u; i < ACTOR_COUNT; i++) {
        if (actors[i].active && actor_visible_in_current_area(&actors[i])) {
            set_actor_frame(&actors[i]);
            move_actor_sprite(&actors[i]);
        } else {
            hide_actor_sprite(&actors[i]);
        }
    }
}

static void load_actor_sprite_data_safe(UINT8 tile_base, const unsigned char *tiles) {
#if SAFE_PER_FRAME_ACTOR_LOAD
    UINT8 frame;
    const unsigned char *frame_tiles;

    /* Actor sheets are 4 directions x 2 walk frames.
     * One 16x16 frame = 4 OBJ tiles = 64 bytes.
     *
     * rpg231 accidentally passed the same base pointer for every frame:
     *   set_banked_sprite_data(tile_base + frame*4, 4, tiles, bank)
     * This made all eight frame slots contain frame 0, so changing
     * player_dir / walk_frame could not change the displayed sprite.
     *
     * Keep the safe 4-tile chunked VRAM transfer, but advance the pointer
     * to the matching frame in the banked sheet.
     */
    for (frame = 0u; frame < (UINT8)(ACTOR_TOTAL_TILES / ACTOR_TILE_COUNT); frame++) {
        frame_tiles = tiles + ((UINT16)frame * (UINT16)ACTOR_TILE_COUNT * 16u);
        set_banked_sprite_data((UINT8)(tile_base + frame * ACTOR_TILE_COUNT), ACTOR_TILE_COUNT, frame_tiles, BANK(sprite_data_bank));
    }
#else
    set_banked_sprite_data(tile_base, ACTOR_TOTAL_TILES, tiles, BANK(sprite_data_bank));
#endif
}

static void init_map_sprites(void) {
    SPRITES_8x8;
    
    load_actor_sprite_data_safe(PLAYER_TILE_BASE, player_tiles);
    load_actor_sprite_data_safe(NPC0_TILE_BASE,   npc_tiles);
    load_actor_sprite_data_safe(ENEMY0_TILE_BASE, enemy_tiles);
    draw_all_actors();
}


static void restore_field_vram_state(void) {
    DISPLAY_OFF;
    HIDE_WIN;
    SHOW_BKG;
    SHOW_SPRITES;

    
    hide_all_sprites_safe();

    
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    jp_init();
    init_map_sprites();

    DISPLAY_ON;
}

static void load_map_mode(void) {
    DISPLAY_OFF;
    SHOW_BKG;
    SHOW_SPRITES;
    HIDE_WIN;
    hide_all_sprites_safe();
    snap_camera_to_player();
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    init_map_sprites();
    DISPLAY_ON;
}

static UINT8 map16_is_blocked(UINT8 tx, UINT8 ty) {
    if (tx >= MAP16_W || ty >= MAP16_H) return 1u;
    return current_collision16_at(tx, ty);
}

static UINT8 actor_at_tile(UINT8 tx, UINT8 ty, UINT8 *enemy_index) {
    UINT8 i;
    *enemy_index = 0xFFu;
    for (i = 0u; i < ACTOR_COUNT; i++) {
        if (!actors[i].active || !actors[i].solid) continue;
        if (!actor_visible_in_current_area(&actors[i])) continue;
        if (actors[i].tx == tx && actors[i].ty == ty) {
            if (actors[i].kind == ACTOR_KIND_ENEMY) *enemy_index = i;
            return 1u;
        }
    }
    return 0u;
}

static Direction opposite_dir(Direction dir) {
    switch (dir) {
        case DIR_UP: return DIR_DOWN;
        case DIR_DOWN: return DIR_UP;
        case DIR_LEFT: return DIR_RIGHT;
        default: return DIR_LEFT;
    }
}

static INT8 find_talkable_actor(void) {
    UINT8 tx = player_tx;
    UINT8 ty = player_ty;
    UINT8 i;

    switch (player_dir) {
        case DIR_UP:
            if (ty == 0u) return -1;
            ty--;
            break;
        case DIR_DOWN:
            ty++;
            break;
        case DIR_LEFT:
            if (tx == 0u) return -1;
            tx--;
            break;
        case DIR_RIGHT:
            tx++;
            break;
        default:
            break;
    }

    for (i = 0u; i < ACTOR_COUNT; i++) {
        if (!actors[i].active || !actors[i].talkable) continue;
        if (!actor_visible_in_current_area(&actors[i])) continue;
        if (actors[i].tx == tx && actors[i].ty == ty) return (INT8)i;
    }
    return -1;
}

static void warp_player_to_tile(UINT8 tx, UINT8 ty, Direction dir) {
    player_moving = 0u;
    move_pixels_remaining = 0u;
    walk_frame = 0u;
    walk_anim_counter = 0u;

    player_tx = tx;
    player_ty = ty;
    player_target_tx = tx;
    player_target_ty = ty;
    player_px = actor_px(tx);
    player_py = actor_py(ty);
    player_target_px = player_px;
    player_target_py = player_py;
    player_dir = dir;

    snap_camera_to_player();
    
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    draw_all_actors();
}

#define MSG_NONE_LOCAL 0xFFu

void change_area_marker(UINT8 before_msg, UINT8 area, UINT8 music, UINT8 tx, UINT8 ty, UINT8 dir, UINT8 after_msg) {
    if (before_msg != MSG_NONE_LOCAL) message_show(before_msg);
    current_area = area;
    actor_runtime_apply_area_npcs(current_area);
    audio_play_music(music);
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;
    warp_player_to_tile(tx, ty, (Direction)dir);
    if (after_msg != MSG_NONE_LOCAL) message_show(after_msg);
    draw_all_actors();
}

void enter_itil_tower_marker(void) {
    itil_tower_start();
    change_area_marker(MSG_NONE_LOCAL, AREA_TOWER, AUDIO_TRACK_DUNGEON, 2u, 13u, DIR_UP, MSG_NONE_LOCAL);
    itil_tower_show_floor_banner();
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    draw_all_actors();
}

void leave_itil_tower_marker(void) {
    change_area_marker(MSG_DUNGEON_EXIT, AREA_FIELD, AUDIO_TRACK_FIELD, 8u, 14u, DIR_DOWN, MSG_NONE_LOCAL);
}

void advance_itil_tower_marker(void) {
    UINT8 reward;
    if (!itil_tower_can_advance()) {
        itil_tower_show_need_answer();
        field_map_render_runtime_draw(current_area);
        apply_camera_scroll();
        draw_all_actors();
        return;
    }
    if (itil_tower_get_floor() >= 100u) {
        reward = itil_tower_grant_reward(random_u8());
        itil_tower_show_reward(reward);
        leave_itil_tower_marker();
        return;
    }
    itil_tower_next_floor();
    warp_player_to_tile(2u, 13u, DIR_UP);
    itil_tower_show_floor_banner();
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    draw_all_actors();
}


static UINT8 field_random_encounter_should_start(void) {
    return field_feature_random_encounter_should_start_runtime(
        current_area,
        (UINT8)(game_mode == MODE_MAP && !dialogue_is_active() && !player_moving),
        player_tx,
        player_ty,
        &encounter_grace_steps,
        &rand_seed,
        RANDOM_ENCOUNTER_RATE
    );
}


static void check_step_event(void) {
    UINT8 event_id;
    event_id = map_event_at_tile(player_tx, player_ty);
    if (map_event_runtime_apply_transition(event_id, current_area)) return;
    if (event_id == MAP_EVENT_HEAL_SPRING) {
        field_feature_activate_heal_spring_runtime();
        draw_all_actors();
        return;
    }
    if (event_id == MAP_EVENT_CAVE_BOSS) {
        inspect_map_event(event_id);
        return;
    }
    if (event_id != MAP_EVENT_NONE) return;

    if ((current_area == AREA_DUNGEON || current_area == AREA_RUINS || current_area == AREA_TOWER || current_area == AREA_CAVE_1 || current_area == AREA_CAVE_2 || current_area == AREA_CAVE_3 || current_area == AREA_CAVE_4) && current_object16_at(player_tx, player_ty) == 5u) {
        party_damage_active(0u, 2u);
        message_show(MSG_TRAP_DAMAGE);
        draw_all_actors();
        return;
    }

    if (field_random_encounter_should_start()) {
        game_mode = MODE_BATTLE;
        player_moving = 0u;
        init_random_battle_from_field();
        enter_battle_screen();
    }
}

static void inspect_map_event(UINT8 event_id) {
    if (map_event_runtime_apply_transition(event_id, current_area)) {
        return;
    } else if (event_id == MAP_EVENT_CHEST) {
        UINT8 flag_id;
        flag_id = field_feature_chest_flag(current_area);
        if (check_event_flag(flag_id)) {
            message_show(MSG_TREASURE_EMPTY);
        } else {
            set_event_flag(flag_id);
            inventory_add(ITEM_POTION, 1u);
            message_show(MSG_TREASURE_FOUND);
            field_map_render_runtime_draw(current_area);
            draw_all_actors();
        }
    } else if (event_id == MAP_EVENT_CAVE_BOSS) {
        if (check_event_flag(FLAG_CAVE_BOSS)) {
            message_show(MSG_TREASURE_EMPTY);
        } else {
            cave_boss_battle_pending = 1u;
            game_mode = MODE_BATTLE;
            player_moving = 0u;
            current_enemy_index = 0xFFu;
            battle_data_load_boss(battle_enemy_data_slots, &battle_enemy_count);
            if (battle_enemy_count == 0u) battle_enemy_count = 1u;
            battle_copy_enemy_from_data(0u);
            battle_prepare_enemy_rank();
            battle_target_index = 0u;
            battle_select_first_alive();
            menu_index = 0u;
            battle_guard_flags = 0u;
            battle_state = BSTATE_PLAYER;
            enter_battle_screen();
        }
    } else if (event_id == MAP_EVENT_RUIN_LORE) {
        message_show(MSG_RUIN_LORE);
    } else if (event_id == MAP_EVENT_HEAL_SPRING) {
        field_feature_activate_heal_spring_runtime();
        draw_all_actors();
    } else if (event_id == MAP_EVENT_SAVE_POINT) {
        save_bridge_build_snapshot(&save_snapshot_work);
        save_runtime_save_select(&save_snapshot_work);
        restore_field_vram_state();
    } else if (event_id == MAP_EVENT_TOWN_VILLAGER) {
        message_show(MSG_TOWN_VILLAGER);
    } else if (event_id == MAP_EVENT_ITIL_TOWER_NPC) {
        if (!itil_tower_ask_question()) {
            itil_tower_show_wrong_exit();
            leave_itil_tower_marker();
        } else {
            field_map_render_runtime_draw(current_area);
            apply_camera_scroll();
            draw_all_actors();
        }
    } else if (shop_runtime_handle_event(event_id)) {
        restore_field_vram_state();
    }
}

static void try_interact(void) {
    INT8 index;
    UINT8 tx;
    UINT8 ty;
    UINT8 event_id;
    index = find_talkable_actor();
    if (index >= 0) {
        actors[(UINT8)index].dir = opposite_dir(player_dir);
        draw_all_actors();
        event_id = map_event_at_tile(actors[(UINT8)index].tx, actors[(UINT8)index].ty);
        if (event_id != MAP_EVENT_NONE) {
            inspect_map_event(event_id);
            return;
        }
        if ((current_area == AREA_TOWN || current_area == AREA_PORT) && check_event_flag(FLAG_ENEMY_DEFEATED)) {
            message_show(MSG_ENEMY_DEFEATED_REVIVE);
            prompt_enemy_revive_choice();
        } else {
            message_show(actors[(UINT8)index].message_id);
        }
        return;
    }

    tx = player_tx;
    ty = player_ty;
    switch (player_dir) {
        case DIR_UP:    if (ty > 0u) ty--; break;
        case DIR_DOWN:  ty++; break;
        case DIR_LEFT:  if (tx > 0u) tx--; break;
        case DIR_RIGHT: tx++; break;
        default: break;
    }
    if (current_object16_at(tx, ty) == 4u) {
        inspect_map_event(MAP_EVENT_CHEST);
        return;
    }
    event_id = map_event_at_tile(tx, ty);
    if (event_id != MAP_EVENT_NONE) inspect_map_event(event_id);
}

static void start_move(Direction dir) {
    UINT8 next_tx;
    UINT8 next_ty;
    UINT8 enemy_index;

    if (dialogue_is_active() || player_moving) return;

    next_tx = player_tx;
    next_ty = player_ty;
    player_dir = dir;

    switch (dir) {
        case DIR_UP:
            if (next_ty == 0u) { draw_all_actors(); return; }
            next_ty--;
            break;
        case DIR_DOWN:
            next_ty++;
            break;
        case DIR_LEFT:
            if (next_tx == 0u) { draw_all_actors(); return; }
            next_tx--;
            break;
        case DIR_RIGHT:
            next_tx++;
            break;
        default:
            return;
    }

    enemy_index = 0xFFu;
    if (map16_is_blocked(next_tx, next_ty)) { draw_all_actors(); return; }
    if (actor_at_tile(next_tx, next_ty, &enemy_index)) {
        if (enemy_index != 0xFFu) {
            game_mode = MODE_BATTLE;
            player_moving = 0u;
            init_battle_from_enemy(enemy_index);
            enter_battle_screen();
        }
        else draw_all_actors();
        return;
    }

    player_target_tx = next_tx;
    player_target_ty = next_ty;
    player_target_px = actor_px(next_tx);
    player_target_py = actor_py(next_ty);
    player_vx = 0;
    player_vy = 0;
    if (dir == DIR_LEFT) player_vx = -1;
    else if (dir == DIR_RIGHT) player_vx = 1;
    else if (dir == DIR_UP) player_vy = -1;
    else if (dir == DIR_DOWN) player_vy = 1;

    player_moving = 1u;
    move_pixels_remaining = PLAYER_STEP_PIXELS;
    walk_anim_counter = 0u;
    walk_frame = 0u;
    draw_all_actors();
}

static void update_player_movement(void) {
    if (!player_moving) return;

    player_px = (UINT8)(player_px + player_vx);
    player_py = (UINT8)(player_py + player_vy);
    if (move_pixels_remaining > 0u) move_pixels_remaining--;
    if (party_debug_escape_accessory_equipped() && move_pixels_remaining > 0u) {
        player_px = (UINT8)(player_px + player_vx);
        player_py = (UINT8)(player_py + player_vy);
        move_pixels_remaining--;
    }

    walk_anim_counter++;
    if (walk_anim_counter >= WALK_ANIM_FRAMES) {
        walk_anim_counter = 0u;
        walk_frame ^= 1u;
    }

    set_player_frame(player_dir, walk_frame);
    move_player_sprite_px(screen_x_from_world(player_px), screen_y_from_world(player_py));

    if (move_pixels_remaining == 0u) {
        player_moving = 0u;
        player_tx = player_target_tx;
        player_ty = player_target_ty;
        player_px = player_target_px;
        player_py = player_target_py;
        walk_frame = 0u;
        walk_anim_counter = 0u;
        set_player_frame(player_dir, walk_frame);
        move_player_sprite_px(screen_x_from_world(player_px), screen_y_from_world(player_py));
        draw_all_actors();
        check_step_event();
    }
}

static void field_overlay_hide(void) {
    if (!map_overlay_visible) return;
    map_overlay_visible = 0u;
    field_map_render_runtime_draw(current_area);
    apply_camera_scroll();
    draw_all_actors();
}

static void field_overlay_show(void) {
    if (map_overlay_visible) return;
    map_overlay_visible = 1u;
    field_overlay_runtime_show(current_area, player_tx, player_ty, camera_px, camera_py);
}

static void map_input(void) {
    UINT8 keys;
    UINT8 new_keys;

    update_player_movement();
    if (update_camera_for_player()) {
        draw_all_actors();
    }

    keys = joypad();
    new_keys = (UINT8)(keys & (UINT8)(~prev_keys));

    if (keys != 0u || player_moving || dialogue_is_active()) {
        map_idle_frames = 0u;
        if (map_overlay_visible) field_overlay_hide();
    } else if (!map_overlay_visible) {
        if (map_idle_frames < 130u) map_idle_frames++;
        if (map_idle_frames >= 120u) field_overlay_show();
    }

    if (dialogue_is_active()) {
        prev_keys = keys;
        return;
    }

    if (new_keys & J_START) {
        open_main_menu();
        prev_keys = joypad();
        return;
    }

    if (player_moving) {
        prev_keys = keys;
        return;
    }

    if (new_keys & J_A) {
        try_interact();
        prev_keys = joypad();
        return;
    }

    if (keys & J_LEFT) start_move(DIR_LEFT);
    else if (keys & J_RIGHT) start_move(DIR_RIGHT);
    else if (keys & J_UP) start_move(DIR_UP);
    else if (keys & J_DOWN) start_move(DIR_DOWN);

    prev_keys = keys;
}

static UINT16 calc_attack_damage(const Fighter *attacker, const Fighter *defender) {
    INT16 damage;

    damage = (INT16)((UINT16)attacker->attack * 2u);
    damage -= (INT16)defender->defense;
    if (damage < 1) return 1u;
    return (UINT16)damage;
}


static char battle_enemy_name_lines[3][20];

static void copy_enemy_line(UINT8 line, const char *name, UINT8 count) {
    UINT8 i;
    UINT8 max;

    if (line >= 3u) return;
    i = 0u;
    max = (count >= 2u) ? 16u : 19u;
    while (name != 0 && name[i] != '\0' && i < max) {
        battle_enemy_name_lines[line][i] = name[i];
        i++;
    }
    if (count >= 2u && i < 17u) {
        battle_enemy_name_lines[line][i++] = 'x';
        battle_enemy_name_lines[line][i++] = (char)('0' + count);
    }
    battle_enemy_name_lines[line][i] = '\0';
}

static void draw_battle_enemy_names(void) {
    UINT8 i;
    UINT8 j;
    UINT8 line;
    UINT8 count;
    UINT8 used[BATTLE_MAX_ENEMY_COUNT];

    draw_bkg_box(0u, 13u, 9u, 5u);
    battle_bkg_clear_area(1u, 14u, 7u, 3u);

    for (i = 0u; i < BATTLE_MAX_ENEMY_COUNT; i++) used[i] = 0u;

    line = 0u;
    for (i = 0u; i < battle_enemy_count && line < 3u; i++) {
        if (used[i] || enemy_battles[i].hp == 0u) continue;
        count = 1u;
        used[i] = 1u;
        for (j = (UINT8)(i + 1u); j < battle_enemy_count; j++) {
            if (enemy_battles[j].hp > 0u &&
                battle_enemy_sprite_kinds[j] == battle_enemy_sprite_kinds[i] &&
                battle_enemy_size_kinds[j] == battle_enemy_size_kinds[i]) {
                used[j] = 1u;
                count++;
            }
        }
        copy_enemy_line(line, enemy_battles[i].name, count);
        battle_put_bkg_text(1u, (UINT8)(14u + line), battle_enemy_name_lines[line]);
        line++;
    }
}


static void hide_all_sprites_safe(void) {
    UINT8 i;
    for (i = 0u; i < 40u; i++) {
        move_sprite(i, 0u, 0u);
    }
}


static void show_party_icon_16(UINT8 sprite_base, UINT8 tile_base, UINT8 x, UINT8 y) {
    
    set_sprite_tile(sprite_base + 0u, tile_base + 0u);
    set_sprite_tile(sprite_base + 1u, tile_base + 2u);

    move_sprite(sprite_base + 0u, (UINT8)(x + 8u),  (UINT8)(y + 16u));
    move_sprite(sprite_base + 1u, (UINT8)(x + 16u), (UINT8)(y + 16u));
}

static void battle_place_party_icons(void) {
    
    show_party_icon_16(BATTLE_PARTY_ICON0_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 0u),  40u, 16u);
    show_party_icon_16(BATTLE_PARTY_ICON1_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 8u),  88u, 16u);
    show_party_icon_16(BATTLE_PARTY_ICON2_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 4u), 136u, 16u);
}




static void battle_refresh_enemy_sprites_compact(UINT8 hide_first) {
    UINT8 alive_flags[BATTLE_MAX_ENEMY_COUNT];
    UINT8 i;

    (void)hide_first;


    for (i = 0u; i < BATTLE_MAX_ENEMY_COUNT; i++) {
        alive_flags[i] = (UINT8)((i < battle_enemy_count && enemy_battles[i].hp > 0u) ? 1u : 0u);
    }

    battle_enemy_bg_draw_all_sized(battle_enemy_count, battle_enemy_sprite_kinds, battle_enemy_size_kinds, alive_flags);
}


static void battle_copy_enemy_from_data(UINT8 slot) {
    enemy_battles[slot].name = battle_enemy_data_slots[slot].name;
    enemy_battles[slot].max_hp = battle_enemy_data_slots[slot].max_hp;
    enemy_battles[slot].hp = battle_enemy_data_slots[slot].hp;
    enemy_battles[slot].attack = battle_enemy_data_slots[slot].attack;
    enemy_battles[slot].defense = battle_enemy_data_slots[slot].defense;
    enemy_battles[slot].skill_power = 0u;
    enemy_battles[slot].heal_power = 0u;
    enemy_battles[slot].agility = battle_enemy_data_slots[slot].agility;
    battle_enemy_sprite_kinds[slot] = battle_enemy_data_slots[slot].sprite_kind;
    if (battle_enemy_sprite_kinds[slot] > 4u) battle_enemy_sprite_kinds[slot] = 0u;
    battle_enemy_size_kinds[slot] = battle_enemy_data_slots[slot].size_kind;
    if (battle_enemy_size_kinds[slot] > BATTLE_ENEMY_SIZE_L) battle_enemy_size_kinds[slot] = BATTLE_ENEMY_SIZE_M;
}

static UINT8 battle_select_first_alive(void) {
    UINT8 i;

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp > 0u) {
            battle_target_index = i;
            return 1u;
        }
    }
    return 0u;
}

static UINT8 battle_ensure_selected_alive(void) {
    if (battle_target_index < battle_enemy_count && enemy_battles[battle_target_index].hp > 0u) {
        return 1u;
    }
    return battle_select_first_alive();
}

static void draw_battle_message_area(void) {
    
    if (battle_message_text == 0 || battle_message_text[0] == '\0') {
        draw_battle_enemy_names();
        draw_battle_menu();
        return;
    }
    battle_hide_command_cursor_obj();
    draw_bkg_box(BATTLE_MSG_X, BATTLE_MSG_Y, BATTLE_MSG_W, BATTLE_MSG_H);
    battle_bkg_clear_area((UINT8)(BATTLE_MSG_X + 1u), (UINT8)(BATTLE_MSG_Y + 1u), (UINT8)(BATTLE_MSG_W - 2u), 3u);
}

static void draw_battle_menu(void) {
    battle_command_ui_runtime_draw();
}

static void battle_move_command_cursor_obj(void) {
    battle_command_ui_runtime_move_cursor(menu_index);
}

static void battle_target_bg_origin(UINT8 slot, UINT8 *col, UINT8 *row) {
    UINT8 size_kind;

    size_kind = battle_enemy_size_kinds[slot];
    if (size_kind == BATTLE_ENEMY_SIZE_S) {
        *col = (UINT8)(2u + (UINT8)(slot % 3u) * 6u);
        *row = (slot < 3u) ? BATTLE_ENEMY_BG_Y : (UINT8)(BATTLE_ENEMY_BG_Y + 3u);
        return;
    }
    if (size_kind == BATTLE_ENEMY_SIZE_L) {
        *col = 4u;
        *row = BATTLE_ENEMY_BG_Y;
        return;
    }

    if (battle_enemy_count <= 1u) {
        *col = 8u;
    } else if (battle_enemy_count == 2u) {
        *col = (slot == 0u) ? 4u : 12u;
    } else {
        if (slot == 0u) *col = 1u;
        else if (slot == 1u) *col = 8u;
        else *col = 15u;
    }
    *row = BATTLE_ENEMY_BG_Y;
}

static void battle_move_target_cursor_obj(void) {
    UINT8 col;
    UINT8 row;

    if (battle_target_index >= battle_enemy_count) {
        battle_select_first_alive();
    }
    battle_target_bg_origin(battle_target_index, &col, &row);
    move_sprite(BATTLE_CURSOR_SPRITE,
                (UINT8)(col * 8u + 4u),
                (UINT8)(row * 8u + 16u));
}

static void battle_hide_command_cursor_obj(void) {
    move_sprite(BATTLE_CURSOR_SPRITE, 0u, 0u);
}

static void update_battle_menu_cursor(UINT8 old_index, UINT8 new_index) {
    (void)old_index;
    (void)new_index;
    battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
    battle_update_dirty();
}

static void battle_flash_enemy_sprite(UINT8 enemy_index) {
    if (enemy_index >= battle_enemy_count) return;

    
    battle_enemy_bg_draw_slot_sized(battle_enemy_count, enemy_index, battle_enemy_sprite_kinds[enemy_index], battle_enemy_size_kinds[enemy_index], 0u);
    audio_wait_vbl();
    audio_wait_vbl();

    if (enemy_battles[enemy_index].hp > 0u) {
        battle_enemy_bg_draw_slot_sized(battle_enemy_count, enemy_index, battle_enemy_sprite_kinds[enemy_index], battle_enemy_size_kinds[enemy_index], 1u);
    }
}

static void battle_reset_bg_origin(void) {
    
    move_bkg(0u, 0u);
    SCX_REG = 0u;
    SCY_REG = 0u;
}

static void battle_hide_window_and_reset_scroll(void) {
    HIDE_WIN;
    move_win(JP_WIN_X, 144u);
    battle_reset_bg_origin();
    SHOW_BKG;
    SHOW_SPRITES;
}

static void battle_set_message_dirty(const char *text) {
    battle_message_text = text;
    battle_dirty_flags |= BATTLE_DIRTY_MESSAGE;
}

static void battle_update_dirty(void) {
    UINT8 flags;

    if (!battle_screen_ready) return;

    
    battle_hide_window_and_reset_scroll();

    flags = battle_dirty_flags;
    battle_dirty_flags = BATTLE_DIRTY_NONE;

    if (flags & (UINT8)(BATTLE_DIRTY_PARTY_HP | BATTLE_DIRTY_PARTY_MP)) {
        battle_status_ui_runtime_draw_party_status_box();
        battle_place_party_icons();
    }
    if (flags & BATTLE_DIRTY_ENEMY_OAM) {
        if (battle_message_text == 0 || battle_message_text[0] == '\0') {
            draw_battle_enemy_names();
        }
        battle_refresh_enemy_sprites_compact(1u);
    }
    if (flags & BATTLE_DIRTY_MESSAGE) {
        draw_battle_message_area();
        if (battle_message_text != 0 && battle_message_text[0] != '\0') {
            battle_put_bkg_text(1u, 14u, battle_message_text);
        } else if (battle_state == BSTATE_PLAYER) {
            battle_move_command_cursor_obj();
        }
    }
    if (flags & BATTLE_DIRTY_CURSOR) {
        if (battle_message_text == 0 || battle_message_text[0] == '\0') {
            battle_move_command_cursor_obj();
        }
    }

    audio_wait_vbl();
}

static void battle_prepare_first_command_ui(void) {
    
    battle_hide_window_and_reset_scroll();

    battle_message_text = "";
    draw_battle_message_area();

    battle_place_party_icons();
    battle_refresh_enemy_sprites_compact(0u);
    battle_move_command_cursor_obj();
}

void battle_show_message(const char *text) {
    battle_set_message_dirty(text);
    battle_update_dirty();
    audio_waitpadup();
    audio_waitpad(J_A);
    audio_waitpadup();
    battle_set_message_dirty("");
    battle_update_dirty();
}

void battle_show_damage_message(const char *prefix, UINT16 value) {
    battle_show_message(battle_format_damage_message(prefix, value));
}

static void battle_enter_render_once(void) {
    
    DISPLAY_OFF;
    battle_screen_ready = 0u;
    battle_dirty_flags = BATTLE_DIRTY_NONE;
    battle_message_text = "";
    battle_first_player_refresh_pending = 0u;

    hide_all_sprites_safe();
    battle_hide_window_and_reset_scroll();
    SPRITES_8x16;
    jp_init();
    battle_enemy_bg_load_tiles_for_formation(battle_enemy_count, battle_enemy_sprite_kinds, battle_enemy_size_kinds);
    set_banked_sprite_data(BATTLE_PARTY_ICON_TILE_BASE, 12u, battle_party_display_tiles, BANK(sprite_data_bank));
    set_banked_sprite_data(BATTLE_CURSOR_TILE, 2u, battle_cursor_tiles, BANK(sprite_data_bank));
    set_sprite_tile(BATTLE_CURSOR_SPRITE, BATTLE_CURSOR_TILE);

    battle_hide_window_and_reset_scroll();
    battle_status_ui_runtime_clear_full();
    battle_status_ui_runtime_draw_party_status_box();
    draw_battle_message_area();
    battle_place_party_icons();
    battle_refresh_enemy_sprites_compact(1u);
    battle_move_command_cursor_obj();
    battle_hide_window_and_reset_scroll();

    battle_screen_ready = 1u;
    DISPLAY_ON;
    audio_wait_vbl();
}

static void update_battle_status(void) {
    battle_dirty_flags |= (UINT8)(BATTLE_DIRTY_PARTY_HP | BATTLE_DIRTY_PARTY_MP | BATTLE_DIRTY_ENEMY_OAM | BATTLE_DIRTY_CURSOR);
    battle_update_dirty();
}

static void init_battle_from_enemy(UINT8 enemy_index) {
    UINT8 i;

    current_enemy_index = enemy_index;
    battle_guard_flags = 0u;
    battle_party_turn_slot = 0u;
    player_battle.name = "ゆうしゃ";
    player_battle.max_hp = player_max_hp_stat;
    player_battle.hp = player_max_hp_stat;
    player_battle.max_mp = player_max_mp_stat;
    player_battle.mp = player_max_mp_stat;
    player_battle.attack = player_attack_stat;
    player_battle.defense = player_defense_stat;
    player_battle.skill_power = player_skill_power_stat;
    player_battle.heal_power = player_heal_power_stat;
    player_battle.magic_mastery = 0u;
    player_battle.agility = player_agility_stat;

    
    battle_data_load_random((UINT8)(enemy_index + 5u), battle_enemy_data_slots, &battle_enemy_count);
    if (battle_enemy_count == 0u) battle_enemy_count = 1u;
    if (battle_enemy_count > BATTLE_MAX_ENEMY_COUNT) battle_enemy_count = BATTLE_MAX_ENEMY_COUNT;

    for (i = 0u; i < battle_enemy_count; i++) {
        battle_copy_enemy_from_data(i);
    }

    battle_prepare_enemy_rank();

    battle_target_index = 0u;
    battle_select_first_alive();

    menu_index = 0u;
    battle_guard_flags = 0u;
    battle_state = BSTATE_PLAYER;
}

static void battle_start_effect(void) {
    HIDE_WIN;
    SHOW_BKG;
    SHOW_SPRITES;
    cgb_fx_battle_start_flash();
}

static void enter_battle_screen(void) {
    
    party_prepare_battle_members(player_max_hp_stat, player_max_mp_stat,
                                 player_attack_stat, player_defense_stat,
                                 player_skill_power_stat, player_heal_power_stat,
                                 player_agility_stat);
    last_growth_type = GROWTH_NONE;
    hide_all_sprites_safe();
    audio_play_music((battle_enemy_count == 1u && battle_enemy_size_kinds[0] == BATTLE_ENEMY_SIZE_L) ? AUDIO_TRACK_BOSS_L : AUDIO_TRACK_BATTLE);
    battle_start_effect();
    battle_enter_render_once();
    battle_show_message(message_get_buffered(MSG_BATTLE_APPEAR));
    battle_first_player_refresh_pending = 1u;
}


static void init_random_battle_from_field(void) {
    UINT8 i;

    current_enemy_index = 0xFFu;

    player_battle.name = "ゆうしゃ";
    player_battle.max_hp = player_max_hp_stat;
    player_battle.hp = player_max_hp_stat;
    player_battle.max_mp = player_max_mp_stat;
    player_battle.mp = player_max_mp_stat;
    player_battle.attack = player_attack_stat;
    player_battle.defense = player_defense_stat;
    player_battle.skill_power = player_skill_power_stat;
    player_battle.heal_power = player_heal_power_stat;
    player_battle.magic_mastery = 0u;
    player_battle.agility = player_agility_stat;

    battle_data_load_random_area(random_u8(), current_area, battle_enemy_data_slots, &battle_enemy_count);
    if (battle_enemy_count == 0u) battle_enemy_count = 1u;
    if (battle_enemy_count > BATTLE_MAX_ENEMY_COUNT) battle_enemy_count = BATTLE_MAX_ENEMY_COUNT;

    for (i = 0u; i < battle_enemy_count; i++) {
        battle_copy_enemy_from_data(i);
    }

    battle_prepare_enemy_rank();

    battle_target_index = 0u;
    battle_select_first_alive();

    menu_index = 0u;
    battle_guard_flags = 0u;
    battle_state = BSTATE_PLAYER;
}


static void battle_prepare_enemy_rank(void) {
    UINT8 i;
    UINT8 rank;
    UINT8 max_rank;

    max_rank = 1u;

    for (i = 0u; i < battle_enemy_count; i++) {
        rank = 1u;

        if (enemy_battles[i].max_hp >= 28u || enemy_battles[i].attack >= 7u) {
            rank = 3u;
        } else if (enemy_battles[i].max_hp >= 20u || enemy_battles[i].attack >= 5u) {
            rank = 2u;
        }

        if (rank > max_rank) {
            max_rank = rank;
        }
    }

    battle_enemy_rank = max_rank;
}

static void return_to_map_after_battle(UINT8 won) {
    game_mode = MODE_MAP;
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;
    
    map_overlay_visible = 0u;

    if (won) {
        if (battle_reward_runtime_apply(battle_enemy_rank, random_u8(), random_u8(), cave_boss_battle_pending)) {
            set_event_flag(FLAG_CAVE_BOSS);
            cave_boss_battle_pending = 0u;
        }
        battle_growth_apply();
        party_after_battle_growth(battle_enemy_rank, random_u8());
        battle_growth_show_message();
        if (current_enemy_index < ACTOR_COUNT &&
            actors[current_enemy_index].kind == ACTOR_KIND_ENEMY) {
            actors[current_enemy_index].active = 0u;
            actors[current_enemy_index].solid = 0u;
            set_event_flag(FLAG_ENEMY_DEFEATED);
        }
    } else {
        cave_boss_battle_pending = 0u;
    }
    restore_field_vram_state();
    audio_play_music(field_feature_music_track(current_area));
}

static void battle_load_current_actor(Fighter *out) {
    PartyBattleFighter src;

    party_get_active_fighter(battle_party_turn_slot, &src);

    out->name = src.name;
    out->max_hp = src.max_hp;
    out->hp = src.hp;
    out->max_mp = src.max_mp;
    out->mp = src.mp;
    out->attack = src.attack;
    out->defense = src.defense;
    out->skill_power = src.skill_power;
    out->heal_power = src.heal_power;
    out->magic_mastery = src.magic_mastery;
    out->agility = src.agility;
}

static UINT8 battle_select_next_party_turn(void) {
    UINT8 slot;

    for (slot = (UINT8)(battle_party_turn_slot + 1u); slot < PARTY_ACTIVE_COUNT; slot++) {
        if (party_get_active_hp(slot) > 0u) {
            battle_party_turn_slot = slot;
            menu_index = CMD_ATTACK;
            return 1u;
        }
    }

    return 0u;
}

static UINT8 battle_select_next_alive_target(UINT8 forward) {
    UINT8 i;
    UINT8 idx;

    if (battle_enemy_count == 0u) return 0u;

    for (i = 0u; i < battle_enemy_count; i++) {
        if (forward) idx = (UINT8)((battle_target_index + 1u + i) % battle_enemy_count);
        else idx = (UINT8)((battle_target_index + battle_enemy_count - 1u - i) % battle_enemy_count);

        if (enemy_battles[idx].hp > 0u) {
            battle_target_index = idx;
            return 1u;
        }
    }

    return 0u;
}

static void battle_start_target_select(UINT8 action_id) {
    battle_pending_action = action_id;
    if (!battle_select_first_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    battle_state = BSTATE_TARGET;
    battle_set_message_dirty("あいてを えらぶ");
    battle_update_dirty();
    battle_move_target_cursor_obj();
}

static void battle_finish_party_action(void) {
    update_battle_status();

    if (!battle_select_first_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    if (battle_select_next_party_turn()) {
        battle_state = BSTATE_PLAYER;
        update_battle_status();
    } else {
        battle_state = BSTATE_ENEMY;
    }
}




static void battle_heal_actor_slot(UINT8 target_slot, UINT16 amount) {
    UINT16 hp;

    hp = party_heal_active(target_slot, amount);
    if (target_slot == 0u) {
        player_battle.hp = hp;
    }
}

static UINT16 battle_use_damage_skill_on_enemies(UINT8 skill_id, const Fighter *actor) {
    UINT8 i;
    UINT16 amount;

    if (!battle_skill_runtime_is_all_target(skill_id)) {
        amount = battle_skill_runtime_calc_damage(skill_id, actor->attack, actor->skill_power, actor->magic_mastery, enemy_battle.defense);
        if (amount >= enemy_battle.hp) enemy_battle.hp = 0u;
        else enemy_battle.hp = (UINT16)(enemy_battle.hp - amount);
        battle_show_damage_message(battle_skill_runtime_is_magic(skill_id) ? "まほう!" : "とくぎ!", amount);
        battle_flash_enemy_sprite(battle_target_index);
        return amount;
    }

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp == 0u) continue;
        amount = battle_skill_runtime_calc_damage(skill_id, actor->attack, actor->skill_power, actor->magic_mastery, enemy_battles[i].defense);
        if (amount >= enemy_battles[i].hp) enemy_battles[i].hp = 0u;
        else enemy_battles[i].hp = (UINT16)(enemy_battles[i].hp - amount);
        battle_flash_enemy_sprite(i);
    }
    battle_refresh_enemy_sprites_compact(0u);
    return 0u;
}

static void battle_apply_spark_skill(UINT8 skill_id, UINT8 is_magic, const Fighter *actor) {
    (void)is_magic;
    party_add_learned_skill(battle_party_turn_slot, skill_id);
    battle_show_message("ひらめいた!");
    battle_show_message(battle_skill_runtime_name_buffered(skill_id));
    (void)battle_use_damage_skill_on_enemies(skill_id, actor);
}



static void player_attack(void) {
    UINT16 dmg;
    Fighter actor;

    if (!battle_ensure_selected_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    battle_load_current_actor(&actor);
    if (battle_party_turn_slot == 0u) last_growth_type = GROWTH_ATK;
    party_battle_op(PARTY_OP_NOTE_ATTACK, battle_party_turn_slot, 0u);
    if (actor.hp == 0u) {
        battle_finish_party_action();
        return;
    }

    dmg = calc_attack_damage(&actor, &enemy_battle);
    if (dmg >= enemy_battle.hp) enemy_battle.hp = 0u;
    else enemy_battle.hp = (UINT16)(enemy_battle.hp - dmg);

    battle_show_damage_message("こうげき!", dmg);
    battle_flash_enemy_sprite(battle_target_index);

    {
        UINT8 spark_skill;
        if (party_try_spark_skill(battle_party_turn_slot, random_u8(), &spark_skill)) {
            battle_apply_spark_skill(spark_skill, 0u, &actor);
        }
    }

    battle_finish_party_action();
}

static void player_use_skill_on_target(UINT8 skill_id, UINT8 ally_slot) {
    UINT16 amount;
    UINT8 mp_cost;
    Fighter actor;
    PartyBattleFighter target;

    if (skill_id == SKILL_NONE) {
        battle_show_message(message_get_buffered(MSG_BATTLE_LOW_MP));
        battle_state = BSTATE_PLAYER;
        update_battle_status();
        return;
    }

    battle_load_current_actor(&actor);
    mp_cost = battle_skill_runtime_mp_cost(skill_id);

    if (battle_skill_runtime_kind(skill_id) == BATTLE_SKILL_KIND_HEAL) {
        party_get_active_fighter(ally_slot, &target);
        if (target.hp >= target.max_hp) {
            battle_show_message(message_get_buffered(MSG_BATTLE_HP_FULL));
            battle_state = BSTATE_PLAYER;
            update_battle_status();
            return;
        }

        if (!((UINT8)party_battle_op(PARTY_OP_TRY_CONSUME_MP, battle_party_turn_slot, (UINT16)mp_cost))) {
            battle_show_message(message_get_buffered(MSG_BATTLE_LOW_MP));
            battle_state = BSTATE_PLAYER;
            update_battle_status();
            return;
        }

        amount = battle_skill_runtime_calc_heal(skill_id, actor.skill_power, actor.magic_mastery);
        if (battle_skill_runtime_is_heal_magic(skill_id)) party_battle_op(PARTY_OP_NOTE_MAGIC, battle_party_turn_slot, 0u);
        if (battle_party_turn_slot == 0u) last_growth_type = GROWTH_HEAL;
        battle_heal_actor_slot(ally_slot, amount);

        battle_show_damage_message("かいふく!", amount);
        if (battle_skill_runtime_is_heal_magic(skill_id)) {
            UINT8 spark_magic;
            if (party_try_spark_magic_from(battle_party_turn_slot, skill_id, random_u8(), &spark_magic)) {
                UINT16 spark_heal;
                party_add_learned_skill(battle_party_turn_slot, spark_magic);
                battle_show_message("まほうを ひらめいた!");
                battle_show_message(battle_skill_runtime_name_buffered(spark_magic));
                if (battle_skill_runtime_kind(spark_magic) == BATTLE_SKILL_KIND_HEAL) {
                    spark_heal = battle_skill_runtime_calc_heal(spark_magic, actor.skill_power, actor.magic_mastery);
                    battle_heal_actor_slot(ally_slot, spark_heal);
                    battle_show_damage_message("かいふく!", spark_heal);
                }
            }
        }
        battle_finish_party_action();
        return;
    }

    if (!battle_ensure_selected_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    if (!((UINT8)party_battle_op(PARTY_OP_TRY_CONSUME_MP, battle_party_turn_slot, (UINT16)mp_cost))) {
        battle_show_message(message_get_buffered(MSG_BATTLE_LOW_MP));
        battle_state = BSTATE_PLAYER;
        update_battle_status();
        return;
    }

    if (battle_party_turn_slot == 0u) last_growth_type = GROWTH_SKILL;
    if (battle_skill_runtime_is_magic(skill_id)) {
        party_battle_op(PARTY_OP_NOTE_MAGIC, battle_party_turn_slot, 0u);
    }
    (void)battle_use_damage_skill_on_enemies(skill_id, &actor);

    if (battle_skill_runtime_is_magic(skill_id)) {
        UINT8 spark_magic;
        if (party_try_spark_magic_from(battle_party_turn_slot, skill_id, random_u8(), &spark_magic)) {
            battle_apply_spark_skill(spark_magic, 1u, &actor);
        }
    } else {
        UINT8 spark_skill;
        if (party_try_spark_skill_from(battle_party_turn_slot, skill_id, random_u8(), &spark_skill)) {
            battle_apply_spark_skill(spark_skill, 0u, &actor);
        }
    }

    battle_finish_party_action();
}



static void player_battle_item(void) {
    UINT8 result;

    battle_hide_command_cursor_obj();
    hide_all_sprites_safe();
    HIDE_SPRITES;
    result = inventory_battle_select_use(battle_party_turn_slot);

    battle_enter_render_once();

    if (result != 2u) {
        battle_state = BSTATE_PLAYER;
        battle_set_message_dirty("");
        battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
        battle_update_dirty();
        return;
    }
    battle_show_message("どうぐを つかった!");
    update_battle_status();
    battle_finish_party_action();
}


static void player_run(void) {
    INT16 chance_i;
    UINT8 chance;
    UINT8 roll;
    Fighter actor;

    battle_load_current_actor(&actor);

    if (party_debug_escape_accessory_equipped()) {
        battle_show_message(message_get_buffered(MSG_BATTLE_ESCAPE_OK));
        battle_state = BSTATE_ESCAPE;
        return;
    }

    chance_i = (INT16)40 + (INT16)actor.agility - (INT16)enemy_battle.agility;
    if (chance_i < 5) chance = 5u;
    else if (chance_i > 95) chance = 95u;
    else chance = (UINT8)chance_i;

    roll = (UINT8)(random_u8() % 100u);
    if (roll < chance) {
        battle_show_message(message_get_buffered(MSG_BATTLE_ESCAPE_OK));
        battle_state = BSTATE_ESCAPE;
    } else {
        battle_show_message(message_get_buffered(MSG_BATTLE_ESCAPE_NG));
        battle_finish_party_action();
    }
}

static void enemy_turn(void) {
    UINT8 i;
    UINT8 target_slot;
    UINT16 dmg;
    Fighter target_fighter;
    PartyBattleFighter party_target;

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp == 0u) continue;
        if (party_active_alive_count() == 0u) {
            battle_state = BSTATE_LOSE;
            return;
        }

        battle_target_index = i;
        target_slot = party_choose_random_alive_active_slot(random_u8());
        party_get_active_fighter(target_slot, &party_target);

        target_fighter.name = party_target.name;
        target_fighter.max_hp = party_target.max_hp;
        target_fighter.hp = party_target.hp;
        target_fighter.max_mp = party_target.max_mp;
        target_fighter.mp = party_target.mp;
        target_fighter.attack = party_target.attack;
        target_fighter.defense = party_target.defense;
        target_fighter.skill_power = party_target.skill_power;
        target_fighter.heal_power = party_target.heal_power;
        target_fighter.agility = party_target.agility;

        dmg = calc_attack_damage(&enemy_battles[i], &target_fighter);
        if (battle_guard_flags & (UINT8)(1u << target_slot)) {
            dmg = (UINT16)((dmg + 1u) >> 1);
        }
        party_damage_active(target_slot, dmg);

        battle_show_damage_message("まものの こうげき!", dmg);
        update_battle_status();

        if (party_active_alive_count() == 0u) {
            battle_state = BSTATE_LOSE;
            return;
        }
    }

    battle_guard_flags = 0u;
    battle_party_turn_slot = 0u;
    if (party_get_active_hp(0u) == 0u) {
        if (!battle_select_next_party_turn()) {
            battle_state = BSTATE_LOSE;
            return;
        }
    }
    battle_select_first_alive();
    battle_state = BSTATE_PLAYER;
}

static void battle_input(void) {
    UINT8 keys;
    UINT8 old_menu_index;

    if (battle_first_player_refresh_pending) {
        battle_first_player_refresh_pending = 0u;
        battle_prepare_first_command_ui();
    }

    if (party_get_active_hp(battle_party_turn_slot) == 0u) {
        if (battle_select_next_party_turn()) {
            update_battle_status();
        } else {
            battle_state = BSTATE_ENEMY;
        }
        return;
    }

    keys = joypad();
    old_menu_index = menu_index;

    if (keys & J_UP) {
        if (menu_index == CMD_ATTACK) menu_index = CMD_DEFEND;
        else if (menu_index == CMD_MAGIC) menu_index = CMD_RUN;
        else menu_index = (UINT8)(menu_index - 2u);
        update_battle_menu_cursor(old_menu_index, menu_index);
        audio_waitpadup();
    } else if (keys & J_DOWN) {
        if (menu_index == CMD_DEFEND) menu_index = CMD_ATTACK;
        else if (menu_index == CMD_RUN) menu_index = CMD_MAGIC;
        else menu_index = (UINT8)(menu_index + 2u);
        update_battle_menu_cursor(old_menu_index, menu_index);
        audio_waitpadup();
    } else if (keys & J_LEFT) {
        if (menu_index & 1u) menu_index--;
        else menu_index++;
        update_battle_menu_cursor(old_menu_index, menu_index);
        audio_waitpadup();
    } else if (keys & J_RIGHT) {
        if (menu_index & 1u) menu_index--;
        else menu_index++;
        update_battle_menu_cursor(old_menu_index, menu_index);
        audio_waitpadup();
    } else if (keys & J_A) {
        audio_waitpadup();
        switch (menu_index) {
            case CMD_ATTACK:
                battle_start_target_select(CMD_ATTACK);
                break;
            case CMD_MAGIC:
                battle_hide_command_cursor_obj();
                battle_skill_runtime_start_magic(battle_party_turn_slot);
                battle_state = BSTATE_SKILL;
                break;
            case CMD_SKILL:
                battle_hide_command_cursor_obj();
                battle_skill_runtime_start(battle_party_turn_slot);
                battle_state = BSTATE_SKILL;
                break;
            case CMD_DEFEND:
                battle_guard_flags |= (UINT8)(1u << battle_party_turn_slot);
                battle_show_message("ぼうぎょ!");
                battle_finish_party_action();
                break;
            case CMD_ITEM:
                player_battle_item();
                break;
            case CMD_RUN:
                player_run();
                break;
            default: break;
        }

        if (battle_state == BSTATE_PLAYER) {
            battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
            battle_update_dirty();
        }
    }
}

static void battle_skill_input(void) {
    UINT8 skill_id;
    UINT8 ev;

    ev = battle_skill_runtime_update(&skill_id);
    if (ev == BATTLE_SKILL_EVENT_CANCEL) {
        battle_state = BSTATE_PLAYER;
        battle_set_message_dirty("");
        battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
        battle_update_dirty();
    } else if (ev == BATTLE_SKILL_EVENT_ENEMY_SKILL) {
        battle_selected_skill = skill_id;
        battle_start_target_select(CMD_SKILL);
    } else if (ev == BATTLE_SKILL_EVENT_ALLY_SKILL) {
        battle_hide_command_cursor_obj();
        battle_skill_runtime_start_ally(skill_id);
        battle_state = BSTATE_ALLY_TARGET;
    }
}

static void battle_ally_target_input(void) {
    UINT8 skill_id;
    UINT8 ally_slot;
    UINT8 ev;

    ev = battle_skill_runtime_update_ally(&skill_id, &ally_slot);
    if (ev == BATTLE_SKILL_EVENT_CANCEL) {
        battle_state = BSTATE_PLAYER;
        battle_set_message_dirty("");
        battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
        battle_update_dirty();
    } else if (ev == BATTLE_SKILL_EVENT_ALLY_SKILL) {
        player_use_skill_on_target(skill_id, ally_slot);
        if (battle_state == BSTATE_PLAYER) {
            battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
            battle_update_dirty();
        }
    }
}

static void battle_target_input(void) {
    UINT8 keys;

    keys = joypad();

    if (keys & (UINT8)(J_LEFT | J_UP)) {
        battle_select_next_alive_target(0u);
        battle_move_target_cursor_obj();
        audio_waitpadup();
    } else if (keys & (UINT8)(J_RIGHT | J_DOWN)) {
        battle_select_next_alive_target(1u);
        battle_move_target_cursor_obj();
        audio_waitpadup();
    } else if (keys & J_B) {
        battle_state = BSTATE_PLAYER;
        battle_set_message_dirty("");
        battle_update_dirty();
        battle_move_command_cursor_obj();
        audio_waitpadup();
    } else if (keys & J_A) {
        audio_waitpadup();
        if (battle_pending_action == CMD_SKILL || battle_pending_action == CMD_MAGIC) {
            player_use_skill_on_target(battle_selected_skill, battle_party_turn_slot);
        } else {
            player_attack();
        }

        if (battle_state == BSTATE_PLAYER) {
            battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
            battle_update_dirty();
        }
    }
}

static void init_game(void) {
    rand_seed = DIV_REG;
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;
    init_event_flags();
    actor_runtime_init_actors();
    party_init_roster_defaults();
    battle_growth_clear();
    battle_enemy_rank = 1u;
    spawn_player_tx = 1u;
    spawn_player_ty = 2u;
    player_tx = spawn_player_tx;
    player_ty = spawn_player_ty;
    player_px = actor_px(player_tx);
    player_py = actor_py(player_ty);
    player_target_tx = player_tx;
    player_target_ty = player_ty;
    player_target_px = player_px;
    player_target_py = player_py;
    player_dir = DIR_DOWN;
    walk_frame = 0u;
    prev_keys = 0u;
    current_enemy_index = 0xFFu;
    current_area = AREA_FIELD;
    actor_runtime_apply_area_npcs(current_area);
    camera_tx = 0u;
    camera_ty = 0u;
    camera_px = 0u;
    camera_py = 0u;
    camera_target_px = 0u;
    camera_target_py = 0u;
    player_max_hp_stat = 38u;
    player_max_mp_stat = 24u;
    player_attack_stat = 7u;
    player_defense_stat = 3u;
    player_skill_power_stat = 8u;
    player_heal_power_stat = 10u;
    player_agility_stat = 8u;
    player_moving = 0u;
    player_vx = 0;
    player_vy = 0;
    move_pixels_remaining = 0u;
    walk_anim_counter = 0u;
    battle_first_player_refresh_pending = 0u;
    battle_guard_flags = 0u;
    battle_party_turn_slot = 0u;
    map_idle_frames = 0u;
    map_overlay_visible = 0u;
    game_mode = MODE_MAP;
}

void main(void) {
    UINT8 title_result;

    BGP_REG = 0xE4u;
    OBP0_REG = 0xE4u;
    OBP1_REG = 0xE4u;

    audio_init();
    dialogue_init();
    title_result = save_runtime_title_load(&save_snapshot_work);
    init_game();
    if (title_result == SAVE_TITLE_CONTINUE) {
        save_bridge_apply_snapshot(&save_snapshot_work);
    }
    load_map_mode();
    audio_play_music(field_feature_music_track(current_area));

    while (1) {
        switch (game_mode) {
            case MODE_MAP:
                map_input();
                break;
            case MODE_BATTLE:
                switch (battle_state) {
                    case BSTATE_PLAYER:
                        battle_input();
                        break;
                    case BSTATE_SKILL:
                        battle_skill_input();
                        break;
                    case BSTATE_TARGET:
                        battle_target_input();
                        break;
                    case BSTATE_ALLY_TARGET:
                        battle_ally_target_input();
                        break;
                    case BSTATE_ENEMY:
                        enemy_turn();
                        if (battle_state == BSTATE_PLAYER) {
                            battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
                            battle_update_dirty();
                        }
                        break;
                    case BSTATE_WIN:
                        battle_show_message(message_get_buffered(MSG_BATTLE_WIN));
                        return_to_map_after_battle(1u);
                        break;
                    case BSTATE_LOSE:
                        battle_show_message(message_get_buffered(MSG_BATTLE_LOSE));
                        player_tx = spawn_player_tx;
                        player_ty = spawn_player_ty;
                        player_px = actor_px(player_tx);
                        player_py = actor_py(player_ty);
                        player_moving = 0u;
                        return_to_map_after_battle(0u);
                        break;
                    case BSTATE_ESCAPE:
                        return_to_map_after_battle(0u);
                        break;
                    default:
                        battle_state = BSTATE_PLAYER;
                        break;
                }
                break;
            default:
                game_mode = MODE_MAP;
                break;
        }
        audio_wait_vbl();
    }
}
