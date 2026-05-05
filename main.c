
#include <gb/gb.h>
#include "dialogue.h"
#include "messages.h"
#include "jpfont.h"
#include "sprites.h"
#include "banked_graphics.h"
#include "map_data.h"
#include "battle_data.h"

/*
 * ============================================================================
 * rpg060 HELPER DEFINITION FIX
 * ============================================================================
 * rpg059 failed at link time because helper functions were declared and used
 * but their definitions were missing from main.c. This version adds the helper
 * bodies without changing BG tiles, OAM, actors, object kinds, or map data.
 * ============================================================================
 */


/*
 * ============================================================================
 * rpg059 NO CONSOLE/STDIO POLICY
 * ============================================================================
 * Bank 0 remains very tight. Do not use the old console text helpers
 * in gameplay screens. Use jpfont helpers and tiny local number conversion.
 * ============================================================================
 */


/*
 * ============================================================================
 * rpg058 MAP DATA BANK POLICY
 * ============================================================================
 * rpg057 still showed romusage warnings for non-banked areas.
 * The 16x16 collision/object arrays are now moved out of main.c into
 * map_data_bank.c (ROM bank 4). main.c reads them through NONBANKED wrappers.
 * ============================================================================
 */

BANKREF_EXTERN(sprite_data_bank)

/*
 * ============================================================================
 * rpg056 GRAPHICS BANK POLICY
 * ============================================================================
 * rpg055 still overflowed from bank 1 into bank 2.
 * The large graphics arrays in sprites.c are now moved to ROM bank 3.
 * main.c must load graphics through banked_graphics.c NONBANKED wrappers.
 * Do not call set_bkg_data()/set_sprite_data() directly for banked assets.
 * ============================================================================
 */



/*
 * ============================================================================
 * GBDK RPG GRAPHICS SAFETY CHECKPOINT (rpg030)
 * ============================================================================
 *
 * This project previously white-screened when graphics were expanded too quickly.
 * The boot path is:
 *
 *   main()
 *     -> dialogue_init()
 *     -> init_game()
 *     -> load_map_mode()
 *          -> DISPLAY_OFF
 *          -> snap_camera_to_player()
 *          -> draw_object_map()
 *               -> build 32x32 BG buffer
 *               -> set_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles)
 *               -> set_bkg_tiles(...)
 *          -> init_map_sprites()
 *               -> set_sprite_data(...) for actor sheets
 *               -> draw_all_actors()
 *          -> DISPLAY_ON
 *
 * If the emulator shows a white screen immediately, suspect the code above
 * BEFORE checking NPC dialogue, town events, battle, or late game logic.
 *
 * Safe modification order:
 *   1. BG tile bytes only, keeping MAP_GFX_TILE_COUNT unchanged.
 *   2. BG tile count increase only, with no object/collision/actor changes.
 *   3. BG overlay placement only, using already-loaded tiles.
 *   4. Object/collision data change only.
 *   5. Actor/NPC slot behavior change only.
 *   6. Sprite/OAM addition only, one OAM group at a time.
 *
 * Static objects such as forest, soft mountains, pots, chests, town signs:
 *   Prefer BG/metatile overlays first.
 *
 * Dynamic objects such as player, NPCs, enemies, cursors, effects:
 *   Use OAM sprites with a fixed slot budget. Do not mix with BG tile numbers.
 *
 * Do not edit draw_object_map(), init_map_sprites(), or MAP_GFX_TILE_COUNT
 * without first reading docs:
 *   GRAPHICS_ARCHITECTURE_030.md
 *   SAFE_GRAPHICS_CHECKPOINT_030.md
 * ============================================================================
 */

/* Map dimension scaffolding.
 * Current safe renderer still draws a 16x16 logical map into a 32x32 BG buffer.
 * Future field expansion must first introduce FIELD_MAP16_W/H and renderer
 * loops that use the active area's width/height. Town size should remain 16x16.
 */
#define FIELD_MAP16_W 16u
#define FIELD_MAP16_H 16u
#define TOWN_MAP16_W 16u
#define TOWN_MAP16_H 16u
#define MAP16_W FIELD_MAP16_W
#define MAP16_H FIELD_MAP16_H
#define TILE16_PX 16u
#define BG_W 20u
#define BG_H 18u
/*
 * Latest v2 flicker fix:
 * Draw the full 32x32 Game Boy BG map once while entering/restoring field.
 * During movement, only move_bkg(camera_px,camera_py) is used.
 * This avoids repeated 22x20 redraws during smooth camera movement, which caused
 * visible flicker on VBA.
 */
#define BG_DRAW_W 32u
#define BG_DRAW_H 32u
#define DRAW_MAP16_W MAP16_W
#define DRAW_MAP16_H MAP16_H
#define SCREEN_PX_W 160u
#define SCREEN_PX_H 144u

/* OAM slot budget.
 * Keep this table updated before adding NPCs, enemies, cursors, effects, or
 * decorative sprites. Static map objects should be BG first, not OAM.
 *
 * Field:
 *  0-3   player 16x16
 *  4-7   npc0 16x16
 *  8-11  enemy0 16x16
 *  12-15 reserved test actor
 * Battle:
 *  0-11  enemies, up to 3 bodies x 16x16
 *  12-23 party icons, 3 members x 16x16
 *  24-39 spare / emergency hide area
 */
#define PLAYER_SPRITE_BASE 0u
#define NPC0_SPRITE_BASE   4u
#define ENEMY0_SPRITE_BASE 8u

/* rpg046 data-only test:
 * Reserve OAM 12-15 through an inactive Actor entry.
 * No new function is added. Existing draw_all_actors() will call
 * hide_actor_sprite() for inactive actors.
 */
#define TEST_ACTOR_SPRITE_BASE 12u
#define BATTLE_PARTY_ICON0_SPRITE 12u
#define BATTLE_PARTY_ICON1_SPRITE 16u
#define BATTLE_PARTY_ICON2_SPRITE 20u
#define BATTLE_PARTY_ICON_TILE_BASE NPC0_TILE_BASE

/* VRAM tile budget.
 * Actor OBJ tile sheets use 0-95.
 * BG tiles start at 96. MAP_GFX_TILE_COUNT controls how many BG tiles are
 * uploaded in draw_object_map(). Keep BG tile IDs and OBJ tile IDs separate.
 */
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

/*
 * ============================================================================
 * rpg071 BATTLE SCENE / ENCOUNTER TABLE POLICY
 * ============================================================================
 * Fix battle screen residue and add a small encounter table.
 *
 * - Full 32x32 battle BG is cleared with already-loaded JP frame blank tile.
 * - Battle command UI uses BG box tiles, matching the message-window style.
 * - Existing enemy OBJ tile sheet is reused for battle enemy sprites.
 * - Enemy data/encounter table lives in battle_data_bank.c bank 7.
 * - Supports up to 3 enemies at once, without adding map actors or map object kinds.
 * - No new BG tiles, MAP_GFX_TILE_COUNT change, object kind, map switch, or
 *   sprite sheet path change.
 * ============================================================================
 */

#define BATTLE_MAX_ENEMY_COUNT BATTLE_DATA_MAX_ENEMIES
#define BATTLE_ENEMY0_SPRITE_BASE 0u
#define BATTLE_ENEMY1_SPRITE_BASE 4u
#define BATTLE_ENEMY2_SPRITE_BASE 8u
#define BATTLE_ENEMY_SPRITE_Y 40u
#define BATTLE_CURSOR_SPRITE 24u
#define BATTLE_CURSOR_TILE 31u
#define BATTLE_MSG_X 0u
#define BATTLE_MSG_Y 15u
#define BATTLE_MSG_W 20u
#define BATTLE_MSG_H 3u

#define BATTLE_DIRTY_NONE      0x00u
#define BATTLE_DIRTY_PARTY_HP  0x01u
#define BATTLE_DIRTY_PARTY_MP  0x02u
#define BATTLE_DIRTY_MESSAGE   0x04u
#define BATTLE_DIRTY_CURSOR    0x08u
#define BATTLE_DIRTY_ENEMY_OAM 0x10u


/*
 * ============================================================================
 * rpg069 FONT CACHE / BATTLE TEXT CLEANUP
 * ============================================================================
 * rpg068 worked but text could corrupt because the temporary byte-based glyph
 * cache key could collide.  jpfont.c now uses the normalized UTF-16 key again.
 *
 * Battle screen cleanup:
 * - reset jpfont cache before drawing battle BG text
 * - redraw battle frame/menu after the opening dialogue closes
 * - avoid decorative ASCII border lines to reduce unnecessary glyph cache use
 * ============================================================================
 */



#define EVENT_FLAG_COUNT 16u
#define FLAG_ENEMY_DEFEATED 0u

#define GROWTH_NONE 0u
#define GROWTH_HP 1u
#define GROWTH_ATK 2u
#define GROWTH_DEF 3u
#define GROWTH_AGI 4u

#define MENU_STATUS 0u
#define MENU_ITEM 1u
#define MENU_EQUIP 2u
#define MENU_OBJECTIVE 3u
#define MENU_COUNT 4u

#define MAP_EVENT_NONE 0u
#define MAP_EVENT_TOWN 1u
#define MAP_EVENT_DUNGEON 2u
#define MAP_EVENT_FIELD_EXIT 3u

#define AREA_FIELD 0u
#define AREA_TOWN 1u

/*
 * ============================================================================
 * rpg068 FIELD RANDOM ENCOUNTER POLICY
 * ============================================================================
 * Field encounters are now step-based random encounters.
 *
 * - Existing ENEMY actor data is used only as a battle template.
 * - Enemy actor OAM is hidden on FIELD and TOWN.
 * - Random encounter check runs only after a completed player step in FIELD.
 * - No new actor entry, OAM slot, BG tile, object kind, or sprite sheet.
 * - Battle start effect is palette flash only; no new VRAM data.
 * ============================================================================
 */


/*
 * ============================================================================
 * rpg067 AREA / ACTOR VISIBILITY POLICY
 * ============================================================================
 * This version adds only area-based actor visibility.
 *
 * AREA_FIELD:
 *   - show ENEMY actors
 *   - hide town NPC actors
 *
 * AREA_TOWN:
 *   - show NPC actors
 *   - hide ENEMY actors
 *
 * This does not add actors, object kinds, BG tiles, sprite sheets, or VRAM load
 * paths. It only changes whether existing actor OAM slots are drawn/blocked/
 * talkable in the current area.
 * ============================================================================
 */


/* rpg008 safe sharp mountain: field impassable visuals use existing kind 2.
 * This is data-only: no new BG tiles, no new kind, no new actor, no new draw branch.
 */

/* Base05 safety toggle.
 * Default is full-sheet loading because player/NPC art was restored from the
 * known-working demo. Flip to 1 only for emergency isolation.
 */
#define SAFE_DUPLICATE_FIRST_ACTOR_FRAME 0


typedef enum GameMode {
    MODE_MAP = 0,
    MODE_BATTLE
} GameMode;

typedef enum BattleState {
    BSTATE_PLAYER = 0,
    BSTATE_ENEMY,
    BSTATE_WIN,
    BSTATE_LOSE,
    BSTATE_ESCAPE
} BattleState;

typedef enum CommandType {
    CMD_ATTACK = 0,
    CMD_SKILL,
    CMD_HEAL,
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
    UINT8 max_hp;
    UINT8 hp;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill_power;
    UINT8 heal_power;
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

/* rpg058: map collision/object arrays moved to map_data_bank.c. */

static Actor actors[] = {
    {ACTOR_KIND_NPC,   1u, 1u, 1u, 3u, 2u, DIR_DOWN, 0u, NPC0_SPRITE_BASE,   NPC0_TILE_BASE,   MSG_COMMON_NPC, {"",0u,0u,0u,0u,0u,0u,0u}},
    {ACTOR_KIND_ENEMY, 1u, 1u, 0u, 7u, 2u, DIR_DOWN, 0u, ENEMY0_SPRITE_BASE, ENEMY0_TILE_BASE, MSG_NONE, {"スライム",20u,20u,5u,2u,0u,0u,5u}},
    /* rpg046:
     * Minimal talkable actor data-only test.
     * Uses OAM 12-15 and existing NPC0_TILE_BASE.
     * active=1 displays the sprite. solid=0 keeps it passable.
     * talkable=1 is tested without changing try_interact().
     */
    {ACTOR_KIND_NPC,   1u, 0u, 1u, 10u, 8u, DIR_DOWN, 0u, TEST_ACTOR_SPRITE_BASE, NPC0_TILE_BASE, MSG_TEST_SHORT, {"",0u,0u,0u,0u,0u,0u,0u}}
};
#define ACTOR_COUNT ((UINT8)(sizeof(actors) / sizeof(actors[0])))

static GameMode game_mode;
static BattleState battle_state;
static UINT8 menu_index;
static UINT8 skill_uses;
static UINT8 heal_uses;
static UINT8 rand_seed;
static UINT8 encounter_grace_steps;
static UINT8 event_flags[EVENT_FLAG_COUNT];
static UINT8 last_growth_type;
/* Japanese UTF-8 battle messages can exceed 48 bytes.
 * Keeping this too small can corrupt nearby globals such as current_enemy_index,
 * which made the NPC disappear after defeating the enemy.
 */
static UINT8 prev_keys;
static UINT8 current_enemy_index;
static UINT8 current_area;
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
static UINT8 battle_enemy_count;
static UINT8 battle_target_index;
static UINT8 battle_dirty_flags;
static const char *battle_message_text;
static UINT8 battle_screen_ready;
#define enemy_battle (enemy_battles[battle_target_index])
static UINT8 player_max_hp_stat;
static UINT8 player_attack_stat;
static UINT8 player_defense_stat;
static UINT8 player_skill_power_stat;
static UINT8 player_heal_power_stat;
static UINT8 player_agility_stat;
static UINT8 player_tx;
static UINT8 player_ty;
static UINT8 spawn_player_tx;
static UINT8 spawn_player_ty;
static UINT8 player_px;
static UINT8 player_py;
static UINT8 player_target_tx;
static UINT8 player_target_ty;
static UINT8 player_target_px;
static UINT8 player_target_py;
static Direction player_dir;
static UINT8 walk_frame;
static UINT8 player_moving;
static INT8 player_vx;
static INT8 player_vy;
static UINT8 move_pixels_remaining;
static UINT8 walk_anim_counter;

static UINT8 min_u8(UINT8 a, UINT8 b);
static UINT8 random_u8(void);
static void init_event_flags(void);
static void set_event_flag(UINT8 id);
static void clear_event_flag(UINT8 id);
static UINT8 check_event_flag(UINT8 id);
static UINT8 actor_px(UINT8 tx);
static UINT8 actor_py(UINT8 ty);
static INT16 screen_x_from_world(UINT8 px);
static INT16 screen_y_from_world(UINT8 py);
static UINT8 camera_max_px_x(void);
static UINT8 camera_max_px_y(void);
static UINT8 camera_tile_from_px(UINT8 px);
static UINT8 camera_sub_from_px(UINT8 px);
static UINT8 step_toward_u8(UINT8 current, UINT8 target, UINT8 step);
static void update_camera_target_for_player(void);
static void apply_camera_scroll(void);
static void snap_camera_to_player(void);
static UINT8 update_camera_for_player(void);
static UINT8 wait_choice_ab(void);
static void revive_enemy_actor(void);
static void prompt_enemy_revive_choice(void);
static void open_main_menu(void);
static void show_status_page(void);
static void show_simple_page(const char *title, const char *line1, const char *line2);
static void put_ascii(UINT8 col, UINT8 row, const char *text);
static void put_cursor(UINT8 col, UINT8 row, UINT8 visible);
static void u8_to_dec(UINT8 value, char *out);
static void put_u8(UINT8 col, UINT8 row, UINT8 value);
static void put_hp_pair(UINT8 col, UINT8 row, UINT8 hp, UINT8 max_hp);
static void screen_clear(void);
static void wait_a_pressed(void);
static void draw_object_map(void);
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
static void reload_field_bg_tiles(void);
static void restore_field_vram_state(void);
static void load_map_mode(void);
static UINT8 map16_is_blocked(UINT8 tx, UINT8 ty);
static UINT8 actor_at_tile(UINT8 tx, UINT8 ty, UINT8 *enemy_index);
static Direction opposite_dir(Direction dir);
static INT8 find_talkable_actor(void);
static UINT8 map_event_at_tile(UINT8 tx, UINT8 ty);
static void warp_player_to_tile(UINT8 tx, UINT8 ty, Direction dir);
static void enter_town_marker(void);
static void leave_town_marker(void);
static void check_step_event(void);
static void inspect_map_event(UINT8 event_id);
static void try_interact(void);

/*
 * ============================================================================
 * rpg055 BANKED MESSAGE POLICY
 * ============================================================================
 * rpg050 white-screened with:
 *   Warning: Write from one bank spans into the next. 0x7fff -> 0x8001
 *
 * Therefore, new dialogue strings must not be added to main.c.
 * Dialogue text is moved to messages_bank.c and copied to WRAM by
 * message_show() before rendering. main.c should keep message IDs only.
 * ============================================================================
 */

/*
 * ============================================================================
 * rpg049 SAFE EVENT POLICY
 * ============================================================================
 *
 * rpg048 white-screened, but rpg048b worked.
 *
 * Current interpretation:
 * - The added actor with talkable=1 is safe when implemented as data-only.
 * - The risky part is likely code/string/layout growth around try_interact(),
 *   not the OAM 12-15 actor slot itself.
 *
 * For now, safe feature additions must prefer:
 *   1. Existing actor/message flow.
 *   2. Existing BG tiles only.
 *   3. Existing object kinds only.
 *   4. No new sprite sheet.
 *   5. No new MAP_GFX_TILE_COUNT.
 *   6. No set_bkg_data()/set_sprite_data() changes.
 *
 * Avoid until explicitly isolated:
 *   - Adding new long Japanese strings and new branching logic together.
 *   - Adding actor entries and interaction logic in the same build.
 *   - Adding BG graphics and event logic in the same build.
 *   - Adding WINDOW/console debug visual code.
 * ============================================================================
 */
static void start_move(Direction dir);
static void update_player_movement(void);
static void map_input(void);
static UINT8 calc_damage(UINT8 power, UINT8 atk, UINT8 def);
static void draw_battle_frame(void);
static void battle_clear_bg_full(void);
static void draw_bkg_box(UINT8 x0, UINT8 y0, UINT8 w, UINT8 h);
static void draw_battle_enemy_names(void);
static void draw_party_status_box(void);
static void draw_battle_message_area(void);
static void battle_enter_render_once(void);
static void battle_update_dirty(void);
static void battle_set_message_dirty(const char *text);
static void battle_show_message(const char *text);
static void battle_update_party_hp_dirty(UINT8 slot);
static void battle_update_party_mp_dirty(UINT8 slot);
static void battle_move_command_cursor_obj(void);
static void battle_hide_command_cursor_obj(void);
static void battle_flash_enemy_sprite(UINT8 enemy_index);
static void battle_hide_window_and_reset_scroll(void);
static void hide_battle_enemy_sprites(void);
static void hide_all_sprites_safe(void);
static void load_battle_party_icon_sprite_data(void);
static void show_battle_party_icons(void);
static void show_battle_enemy_sprites(void);
static void load_battle_enemy_sprite_data(void);
static void battle_copy_enemy_from_data(UINT8 slot);
static UINT8 battle_select_first_alive(void);
static UINT8 battle_alive_count(void);
static void draw_battle_menu(void);
static void update_battle_menu_cursor(UINT8 old_index, UINT8 new_index);
static void update_battle_status(void);
static void init_battle_from_enemy(UINT8 enemy_index);
static void enter_battle(UINT8 enemy_index);
static UINT8 field_random_encounter_should_start(void);
static void battle_start_effect(void);
static void enter_battle_screen(void);
static void init_random_battle_from_field(void);
static void enter_random_battle(void);
static void apply_battle_growth(void);
static void show_growth_message(void);
static void return_to_map_after_battle(UINT8 won);
static void player_attack(void);
static void player_skill(void);
static void player_heal(void);
static void player_run(void);
static void enemy_turn(void);
static void battle_input(void);
static void init_game(void);

static UINT8 min_u8(UINT8 a, UINT8 b) { return (a < b) ? a : b; }
static UINT8 random_u8(void) { rand_seed = (UINT8)(rand_seed * 17u + 29u); return rand_seed; }

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

static void clear_event_flag(UINT8 id) {
    if (id < EVENT_FLAG_COUNT) event_flags[id] = 0u;
}

static UINT8 check_event_flag(UINT8 id) {
    if (id < EVENT_FLAG_COUNT) return event_flags[id];
    return 0u;
}
static UINT8 actor_px(UINT8 tx) { return (UINT8)(tx * TILE16_PX); }
static UINT8 actor_py(UINT8 ty) { return (UINT8)(ty * TILE16_PX); }

static INT16 screen_x_from_world(UINT8 px) { return (INT16)((INT16)px - (INT16)camera_px); }
static INT16 screen_y_from_world(UINT8 py) { return (INT16)((INT16)py - (INT16)camera_py); }

static UINT8 camera_max_px_x(void) {
    return (UINT8)((MAP16_W * TILE16_PX > SCREEN_PX_W) ?
                   (MAP16_W * TILE16_PX - SCREEN_PX_W) : 0u);
}

static UINT8 camera_max_px_y(void) {
    return (UINT8)((MAP16_H * TILE16_PX > SCREEN_PX_H) ?
                   (MAP16_H * TILE16_PX - SCREEN_PX_H) : 0u);
}

static UINT8 camera_tile_from_px(UINT8 px) { return (UINT8)(px / TILE16_PX); }
static UINT8 camera_sub_from_px(UINT8 px) { return (UINT8)(px & 15u); }

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

    if (desired_x > camera_max_px_x()) desired_x = camera_max_px_x();
    if (desired_y > camera_max_px_y()) desired_y = camera_max_px_y();

    camera_target_px = desired_x;
    camera_target_py = desired_y;
}

static void apply_camera_scroll(void) {
    /* Full 32x32 BG map mode: scroll by the real camera pixel offset.
     * The old viewport mode scrolled only the low 4 bits and redrew the BG when
     * camera_tx/camera_ty changed. That redraw could flicker during movement.
     */
    move_bkg(camera_px, camera_py);
}

static void snap_camera_to_player(void) {
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

    /* Keep tile coordinates for status/debug only.  Do not redraw the BG here.
     * The full 32x32 BG map is already loaded, so redrawing while DISPLAY_ON is
     * unnecessary and was the source of visible movement flicker.
     */
    camera_tx = camera_tile_from_px(camera_px);
    camera_ty = camera_tile_from_px(camera_py);
    apply_camera_scroll();

    return (UINT8)((old_px != camera_px) || (old_py != camera_py));
}

static UINT8 wait_choice_ab(void) {
    UINT8 keys;
    waitpadup();
    while (1) {
        keys = joypad();
        if (keys & J_A) { waitpadup(); return 1u; }
        if (keys & J_B) { waitpadup(); return 0u; }
        wait_vbl_done();
    }
}

static void revive_enemy_actor(void) {
    actors[ENEMY_ACTOR_INDEX].active = 1u;
    actors[ENEMY_ACTOR_INDEX].solid = 1u;
    actors[ENEMY_ACTOR_INDEX].stats.hp = actors[ENEMY_ACTOR_INDEX].stats.max_hp;
    actors[ENEMY_ACTOR_INDEX].dir = DIR_DOWN;
    actors[ENEMY_ACTOR_INDEX].walk_frame = 0u;
    clear_event_flag(FLAG_ENEMY_DEFEATED);
}

static void prompt_enemy_revive_choice(void) {
    if (actors[ENEMY_ACTOR_INDEX].active) {
        dialogue_message("まだ まものは\nそこに いるよ。");
        return;
    }

    dialogue_message_nowait_window("まものを ふっかつ?\nA:はい  B:いいえ");
    if (wait_choice_ab()) {
        dialogue_hide();
        revive_enemy_actor();
        draw_all_actors();
        dialogue_message("まものが\nふっかつした！");
    } else {
        dialogue_hide();
        dialogue_message("そのままに\nしておこう。");
    }
    draw_all_actors();
}


/*
 * rpg060:
 * Definitions for the lightweight text helpers introduced in rpg059.
 * rpg059 had prototypes/call sites, but the helper function bodies were not
 * emitted into main.c, causing undefined global linker errors.
 */
static void put_ascii(UINT8 col, UINT8 row, const char *text) {
    jp_put_bkg_text(col, row, text);
}

static void put_cursor(UINT8 col, UINT8 row, UINT8 visible) {
    if (visible) {
        jp_put_bkg_text(col, row, ">");
    } else {
        jp_put_bkg_text(col, row, " ");
    }
}

static void u8_to_dec(UINT8 value, char *out) {
    UINT8 hundreds;
    UINT8 tens;
    UINT8 ones;
    UINT8 i = 0u;

    hundreds = value / 100u;
    tens = (value / 10u) % 10u;
    ones = value % 10u;

    if (hundreds != 0u) {
        out[i++] = (char)('0' + hundreds);
        out[i++] = (char)('0' + tens);
    } else if (tens != 0u) {
        out[i++] = (char)('0' + tens);
    }
    out[i++] = (char)('0' + ones);
    out[i] = '\0';
}

static void put_u8(UINT8 col, UINT8 row, UINT8 value) {
    char buf[4];
    u8_to_dec(value, buf);
    jp_put_bkg_text(col, row, buf);
}

static void put_hp_pair(UINT8 col, UINT8 row, UINT8 hp, UINT8 max_hp) {
    char a[4];
    char b[4];
    char buf[9];
    UINT8 i = 0u;
    UINT8 j = 0u;

    u8_to_dec(hp, a);
    u8_to_dec(max_hp, b);

    while (a[j] != '\0') {
        buf[i++] = a[j++];
    }
    buf[i++] = '/';
    j = 0u;
    while (b[j] != '\0') {
        buf[i++] = b[j++];
    }
    buf[i++] = ' ';
    buf[i++] = ' ';
    buf[i] = '\0';

    jp_put_bkg_text(col, row, buf);
}

static void screen_clear(void) {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
}

static void show_status_page(void) {
    screen_clear();
    jp_put_bkg_text(1u, 0u, "つよさ");
    jp_put_bkg_text(0u, 2u, "たいりょく"); put_u8(11u, 2u, player_max_hp_stat);
    jp_put_bkg_text(0u, 3u, "ちから");     put_u8(11u, 3u, player_attack_stat);
    jp_put_bkg_text(0u, 4u, "まもり");     put_u8(11u, 4u, player_defense_stat);
    jp_put_bkg_text(0u, 5u, "とくぎ");     put_u8(11u, 5u, player_skill_power_stat);
    jp_put_bkg_text(0u, 6u, "かいふく");   put_u8(11u, 6u, player_heal_power_stat);
    jp_put_bkg_text(0u, 7u, "すばやさ");   put_u8(11u, 7u, player_agility_stat);
    jp_put_bkg_text(0u,14u, "A/B/START もどる");
}

static void show_simple_page(const char *title, const char *line1, const char *line2) {
    screen_clear();
    jp_put_bkg_text(1u, 0u, title);
    jp_put_bkg_text(0u, 3u, line1);
    jp_put_bkg_text(0u, 5u, line2);
    jp_put_bkg_text(0u,14u, "A/B/START もどる");
}

static void open_main_menu(void) {
    UINT8 keys;
    UINT8 cursor = MENU_STATUS;

    DISPLAY_OFF;
    HIDE_WIN;
    HIDE_SPRITES;
    SHOW_BKG;
    move_bkg(0u, 0u);
    DISPLAY_ON;
    waitpadup();

    while (1) {
        screen_clear();
        jp_put_bkg_text(1u, 0u, "メニュー");
        put_cursor(0u, 3u, (UINT8)(cursor == MENU_STATUS)); jp_put_bkg_text(1u, 3u, "つよさ");
        put_cursor(0u, 4u, (UINT8)(cursor == MENU_ITEM));   jp_put_bkg_text(1u, 4u, "もちもの");
        put_cursor(0u, 5u, (UINT8)(cursor == MENU_EQUIP));  jp_put_bkg_text(1u, 5u, "そうび");
        put_cursor(0u, 6u, (UINT8)(cursor == MENU_OBJECTIVE)); jp_put_bkg_text(1u, 6u, "もくてき");
        jp_put_bkg_text(0u,14u, "A けってい B もどる");

        keys = waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        waitpadup();

        if (keys & J_UP) {
            if (cursor == 0u) cursor = MENU_COUNT - 1u;
            else cursor--;
        } else if (keys & J_DOWN) {
            cursor++;
            if (cursor >= MENU_COUNT) cursor = 0u;
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            if (cursor == MENU_STATUS) {
                show_status_page();
            } else if (cursor == MENU_ITEM) {
                show_simple_page("もちもの", "やくそう x3", "かぎ なし");
            } else if (cursor == MENU_EQUIP) {
                show_simple_page("そうび", "木のけん", "ぬののふく");
            } else {
                if (check_event_flag(FLAG_ENEMY_DEFEATED)) {
                    show_simple_page("もくてき", "まもの とうばつずみ", "NPCに はなすと ふっかつ");
                } else {
                    show_simple_page("もくてき", "まものを たおす", "つぎは まちと どうくつ");
                }
            }
            waitpad(J_A | J_B | J_START);
            waitpadup();
        }
    }

    hide_battle_enemy_sprites();
    restore_field_vram_state();
}

static void wait_a_pressed(void) {
    waitpad(J_A);
    waitpadup();
}

/* GRAPHICS HOTSPOT: draw_object_map()
 *
 * Responsibilities:
 * - Convert current area's 16x16 object map into the 32x32 BG buffer.
 * - Apply visual-only BG overlays such as forest markers.
 * - Upload BG tile data and BG map data.
 *
 * Safe extension:
 * - Add new BG art by appending map_gfx_tiles and increasing
 *   MAP_GFX_TILE_COUNT in sprites.h only after testing the count alone.
 * - Add passable visual objects as overlays here without touching collision.
 *
 * Unsafe combined changes:
 * - Increasing MAP_GFX_TILE_COUNT + changing collision + changing actors.
 * - Adding new object kind + changing town/field switch + adding sprites.
 */
static void draw_object_map(void) {
    UINT8 x;
    UINT8 y;
    static UINT8 bg[BG_DRAW_W * BG_DRAW_H];
    UINT8 sx;
    UINT8 sy;
    UINT8 wx;
    UINT8 wy;
    UINT8 kind;
    UINT8 tl;
    UINT8 tr;
    UINT8 bl;
    UINT8 br;
    UINT16 bg_i;
    UINT16 top_left_i;

    /*
     * Base06 wall rewrite:
     * - Use UINT16 indexes.  The 22x20 buffer has 440 cells, so UINT8 indexes
     *   wrap past 255 and corrupt the lower half of the BG buffer.
     * - Build walls as real 16x16 metatiles using four independent 8x8 tiles.
     *   This replaces the old repeated single-tile wall drawing completely.
     */
    for (y = 0u; y < BG_DRAW_H; y++) {
        for (x = 0u; x < BG_DRAW_W; x++) {
            bg_i = (UINT16)y * (UINT16)BG_DRAW_W + (UINT16)x;
            bg[bg_i] = MAP_TILE_FLOOR;
        }
    }

    for (y = 0u; y < DRAW_MAP16_H; y++) {
        for (x = 0u; x < DRAW_MAP16_W; x++) {
            /* Full-map BG mode: draw world tile x/y directly into the 32x32 BG.
             * Camera movement is handled only by move_bkg(), so no redraw is
             * needed during normal walking.
             */
            wx = x;
            wy = y;
            /* DRAW_MAP16_W/H are equal to MAP16_W/H in full-map BG mode,
             * so the bounds fallback was always unreachable and triggered
             * SDCC warnings 110/126.  The loop range is already safe here.
             */
            kind = current_object16_at(wx, wy);

            sx = (UINT8)(x * 2u);
            sy = (UINT8)(y * 2u);

            if (kind == 1u) {
                tl = MAP_TILE_WALL_TL;
                tr = MAP_TILE_WALL_TR;
                bl = MAP_TILE_WALL_BL;
                br = MAP_TILE_WALL_BR;
            } else if (kind == 2u) {
                tl = MAP_TILE_CAP_TL;
                tr = MAP_TILE_CAP_TR;
                bl = MAP_TILE_CAP_BL;
                br = MAP_TILE_CAP_BR;
            } else if (kind == 3u) {
                /* v3d: visible town entrance marker.  It is still a BG
                 * metatile, not an actor, so it does not consume OAM sprites.
                 */
                tl = MAP_TILE_TOWN_TL;
                tr = MAP_TILE_TOWN_TR;
                bl = MAP_TILE_TOWN_BL;
                br = MAP_TILE_TOWN_BR;
            } else {
                tl = MAP_TILE_FLOOR;
                tr = MAP_TILE_FLOOR;
                bl = MAP_TILE_FLOOR;
                br = MAP_TILE_FLOOR;
            }

            top_left_i = (UINT16)sy * (UINT16)BG_DRAW_W + (UINT16)sx;
            bg[top_left_i] = tl;
            bg[top_left_i + 1u] = tr;
            bg[top_left_i + (UINT16)BG_DRAW_W] = bl;
            bg[top_left_i + (UINT16)BG_DRAW_W + 1u] = br;
        }
    }

    /* rpg022: draw-only forest BG overlay.
     * This is a visual-only overlay:
     * - adds only 4 BG tiles for forest, keeping town marker tiles intact
     * - no new object kind / no collision change
     * - no new object kind
     * - no collision change
     * It reuses existing town-marker BG tiles as a temporary forest marker.
     */
    if (current_area == AREA_FIELD) {
        sx = (UINT8)(FOREST_BG_X * 2u);
        sy = (UINT8)(FOREST_BG_Y * 2u);
        top_left_i = (UINT16)sy * (UINT16)BG_DRAW_W + (UINT16)sx;
        bg[top_left_i] = MAP_TILE_FOREST_TL;
        bg[top_left_i + 1u] = MAP_TILE_FOREST_TR;
        bg[top_left_i + (UINT16)BG_DRAW_W] = MAP_TILE_FOREST_BL;
        bg[top_left_i + (UINT16)BG_DRAW_W + 1u] = MAP_TILE_FOREST_BR;
    }

    set_banked_bkg_data(MAP_TILE_BASE, MAP_GFX_TILE_COUNT, map_gfx_tiles, BANK(sprite_data_bank));
    set_bkg_tiles(0u, 0u, BG_DRAW_W, BG_DRAW_H, bg);
}
static void set_player_frame(Direction dir, UINT8 frame) {
    UINT8 base;
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
    if (current_area == AREA_TOWN) {
        return (UINT8)(actor->kind == ACTOR_KIND_NPC);
    }

    if (current_area == AREA_FIELD) {
        /*
         * rpg068:
         * Field encounters are random step encounters.
         * Existing enemy actor data remains as a battle template only, so all
         * non-player map actors are hidden from the field for now.
         */
        return 0u;
    }

    return 1u;
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
#if SAFE_DUPLICATE_FIRST_ACTOR_FRAME
    UINT8 frame;
    /*
     * Field stability first:
     * duplicate the verified first 16x16 frame into all 8 logical frame slots.
     * This prevents non-down directions / walk frames from using bad or unverified
     * tile patterns while keeping the existing set_*_frame() indexing intact.
     */
    for (frame = 0u; frame < (UINT8)(ACTOR_TOTAL_TILES / ACTOR_TILE_COUNT); frame++) {
        set_banked_sprite_data((UINT8)(tile_base + frame * ACTOR_TILE_COUNT), ACTOR_TILE_COUNT, tiles, BANK(sprite_data_bank));
    }
#else
    set_banked_sprite_data(tile_base, ACTOR_TOTAL_TILES, tiles, BANK(sprite_data_bank));
#endif
}

/* GRAPHICS HOTSPOT: init_map_sprites()
 *
 * This function reloads OBJ graphics for player/NPC/enemy whenever map mode
 * starts or returns from battle/menu. White screens after sprite edits often
 * come from touching this path.
 *
 * Safe extension:
 * - Add one new actor sheet at a time.
 * - Reserve OAM slots first.
 * - Load OBJ tiles with set_sprite_data() only for OBJ sprites.
 *
 * Do not use BG tile numbers as sprite tile numbers.
 */
static void init_map_sprites(void) {
    SPRITES_8x8;
    /*
     * Keep each actor in a non-overlapping 32-tile slot: player 0-31,
     * NPC 32-63, enemy 64-95.  player/NPC are restored from the known-working demo.
     * Full sheets are reloaded here every time map mode is restored.
     */
    load_actor_sprite_data_safe(PLAYER_TILE_BASE, player_tiles);
    load_actor_sprite_data_safe(NPC0_TILE_BASE,   npc_tiles);
    load_actor_sprite_data_safe(ENEMY0_TILE_BASE, enemy_tiles);
    draw_all_actors();
}


static void reload_field_bg_tiles(void) {
    draw_object_map();
    apply_camera_scroll();
}

static void restore_field_vram_state(void) {
    DISPLAY_OFF;
    HIDE_WIN;
    SHOW_BKG;
    SHOW_SPRITES;

    /*
     * rpg069:
     * Restore map BG and reset jpfont cache/window while the LCD is off.
     * The map itself does not use dynamic jpfont cache tiles, so this is safe.
     */
    reload_field_bg_tiles();
    jp_init();
    init_map_sprites();

    DISPLAY_ON;
}

/* BOOT/MAP RESTORE HOTSPOT: load_map_mode()
 *
 * If DISPLAY_ON is not reached, the emulator can remain white.
 * Keep this path boring and deterministic.
 */
static void load_map_mode(void) {
    DISPLAY_OFF;
    SHOW_BKG;
    SHOW_SPRITES;
    HIDE_WIN;
    snap_camera_to_player();
    draw_object_map();
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

static UINT8 map_event_at_tile(UINT8 tx, UINT8 ty) {
    /* v3e-safe: Add town movement without restoring the risky v4/v3e
     * real map-array switch. FIELD -> TOWN and TOWN -> FIELD are both
     * controlled warps inside the already verified 16x16 map.
     */
    if (current_area == AREA_FIELD && tx == 14u && ty == 2u) return MAP_EVENT_TOWN;
    if (current_area == AREA_TOWN && tx == 2u && ty == 14u) return MAP_EVENT_FIELD_EXIT;
    if (current_area == AREA_FIELD && tx == 1u && ty == 14u) return MAP_EVENT_DUNGEON;
    return MAP_EVENT_NONE;
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
    /* v3f: Separate town map arrays need the BG map to be redrawn after an
     * area change.  This still uses the verified 32x32 full-BG redraw path and
     * does not reload sprite data or introduce a new town VRAM pipeline.
     */
    draw_object_map();
    apply_camera_scroll();
    draw_all_actors();
}

static void enter_town_marker(void) {
    dialogue_message("まちへ\nはいります。");

    /* v3f/rpg067:
     * Switch the area selector only. Actor visibility is decided by
     * actor_visible_in_current_area(); no actor array or sprite sheet is changed.
     */
    current_area = AREA_TOWN;
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;
    warp_player_to_tile(2u, 13u, DIR_DOWN);
    dialogue_message("まちに\nつきました。");
    draw_all_actors();
}

static void leave_town_marker(void) {
    dialogue_message("フィールドへ\nもどります。");
    /* rpg067:
     * Returning to field re-enables field-visible actors and hides town NPCs.
     */
    current_area = AREA_FIELD;
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;
    warp_player_to_tile(13u, 2u, DIR_RIGHT);
    dialogue_message("フィールドに\nでました。");
    draw_all_actors();
}

static UINT8 field_random_encounter_should_start(void) {
    UINT8 roll;

    if (current_area != AREA_FIELD) return 0u;
    if (game_mode != MODE_MAP) return 0u;
    if (dialogue_is_active()) return 0u;
    if (player_moving) return 0u;

    /* Do not trigger on special event tiles such as town entrance. */
    if (map_event_at_tile(player_tx, player_ty) != MAP_EVENT_NONE) return 0u;

    if (encounter_grace_steps > 0u) {
        encounter_grace_steps--;
        return 0u;
    }

    /* Stir the simple LCG with DIV_REG so repeated boots do not feel identical. */
    rand_seed ^= DIV_REG;
    roll = random_u8();

    return (UINT8)(roll < RANDOM_ENCOUNTER_RATE);
}

static void check_step_event(void) {
    UINT8 event_id;
    event_id = map_event_at_tile(player_tx, player_ty);
    if (event_id == MAP_EVENT_TOWN) {
        enter_town_marker();
        return;
    } else if (event_id == MAP_EVENT_FIELD_EXIT) {
        leave_town_marker();
        return;
    } else if (event_id != MAP_EVENT_NONE) {
        return;
    }

    if (field_random_encounter_should_start()) {
        enter_random_battle();
    }
}

static void inspect_map_event(UINT8 event_id) {
    if (event_id == MAP_EVENT_TOWN) {
        dialogue_message_nowait_window("まちに はいりますか?\nA:はい  B:いいえ");
        if (wait_choice_ab()) {
            dialogue_hide();
            enter_town_marker();
        } else {
            dialogue_hide();
            dialogue_message("また あとで\nこよう。");
            draw_all_actors();
        }
    } else if (event_id == MAP_EVENT_FIELD_EXIT) {
        dialogue_message_nowait_window("フィールドへ でますか?\nA:はい  B:いいえ");
        if (wait_choice_ab()) {
            dialogue_hide();
            leave_town_marker();
        } else {
            dialogue_hide();
            dialogue_message("もうすこし\nまちにいよう。");
            draw_all_actors();
        }
    } else if (event_id == MAP_EVENT_DUNGEON) {
        dialogue_message("くらい どうくつが\nつづいている。");
    }
}

/* rpg049 note: keep this function small.
 * Do not add new actor branching and new message strings here in the same build.
 * rpg048b proved data-only talkable actors can work; interaction code growth
 * should be isolated separately.
 */
static void try_interact(void) {
    INT8 index;
    UINT8 tx;
    UINT8 ty;
    UINT8 event_id;
    index = find_talkable_actor();
    if (index >= 0) {
        actors[(UINT8)index].dir = opposite_dir(player_dir);
        draw_all_actors();
        if (check_event_flag(FLAG_ENEMY_DEFEATED)) {
            dialogue_message("まものは たおした。\nふっかつ する？");
        } else {
            message_show(actors[(UINT8)index].message_id);
        }
        prompt_enemy_revive_choice();
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
        if (enemy_index != 0xFFu) enter_battle(enemy_index);
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

static void map_input(void) {
    UINT8 keys;
    UINT8 new_keys;

    update_player_movement();
    if (update_camera_for_player()) {
        draw_all_actors();
    }

    keys = joypad();
    new_keys = (UINT8)(keys & (UINT8)(~prev_keys));

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

static UINT8 calc_damage(UINT8 power, UINT8 atk, UINT8 def) {
    UINT8 variance;
    UINT8 base;
    variance = (UINT8)(random_u8() % 4u);
    base = (UINT8)(power + atk);
    if (base <= def) return 1u;
    base = (UINT8)(base - def + variance);
    if (base == 0u) return 1u;
    return base;
}

/*
 * rpg074:
 * Keep battle UI updates in this small helper zone.  Do not add BG tiles or
 * change VRAM load order here.  The goal is to stabilize the first command
 * screen, then expand party/status display one visible slot at a time.
 */
static void battle_clear_bg_full(void) {
    static UINT8 bg[BG_DRAW_W * BG_DRAW_H];
    UINT16 i;
    UINT8 blank;

    /*
     * Clear the full 32x32 BG map, not just the visible 20x18 area.
     * This prevents field BG residue on the first battle command turn.
     */
    blank = (UINT8)(JP_FRAME_BASE + 0u);
    for (i = 0u; i < (UINT16)(BG_DRAW_W * BG_DRAW_H); i++) {
        bg[i] = blank;
    }

    move_bkg(0u, 0u);
    set_bkg_tiles(0u, 0u, BG_DRAW_W, BG_DRAW_H, bg);
}

static void draw_bkg_box(UINT8 x0, UINT8 y0, UINT8 w, UINT8 h) {
    UINT8 x;
    UINT8 y;
    UINT8 t;

    if (w < 2u || h < 2u) return;

    for (y = 0u; y < h; y++) {
        for (x = 0u; x < w; x++) {
            t = JP_FRAME_BASE + 0u;
            if (y == 0u && x == 0u) t = JP_FRAME_BASE + 3u;
            else if (y == 0u && x == (UINT8)(w - 1u)) t = JP_FRAME_BASE + 4u;
            else if (y == (UINT8)(h - 1u) && x == 0u) t = JP_FRAME_BASE + 5u;
            else if (y == (UINT8)(h - 1u) && x == (UINT8)(w - 1u)) t = JP_FRAME_BASE + 6u;
            else if (y == 0u || y == (UINT8)(h - 1u)) t = JP_FRAME_BASE + 1u;
            else if (x == 0u || x == (UINT8)(w - 1u)) t = JP_FRAME_BASE + 2u;

            set_bkg_tiles((UINT8)(x0 + x), (UINT8)(y0 + y), 1u, 1u, &t);
        }
    }
}

static void draw_battle_enemy_names(void) {
    UINT8 i;

    /* rpg082: keep enemy-name box above the bottom message strip.
     * Rows 12-14 hold up to three enemy names; rows 15-17 are message-only.
     */
    draw_bkg_box(0u, 11u, 9u, 4u);
    jp_bkg_clear_area(1u, 12u, 7u, 3u);

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp > 0u) {
            jp_put_bkg_text(1u, (UINT8)(12u + i), enemy_battles[i].name);
        }
    }
}

static void draw_party_status_box(void) {
    /* rpg081:
     * One continuous party strip, no per-character vertical separators.
     * Layout is intentionally compact:
     *   name
     *   H nn
     *   M nn  + 16x16 OBJ icon on the right
     * Keeping the whole party inside the top 20x5 BG area prevents the first
     * command screen from inheriting field BG/window residue.
     */
    draw_bkg_box(0u, 0u, 20u, 5u);

    jp_put_bkg_text(1u, 1u,  "ゆうしゃ");
    jp_put_bkg_text(1u, 2u,  "H ");
    put_u8(3u, 2u, player_battle.hp);
    jp_put_bkg_text(1u, 3u,  "M ");
    put_u8(3u, 3u, 38u);

    jp_put_bkg_text(7u, 1u,  "そうりょ");
    jp_put_bkg_text(7u, 2u,  "H ");
    put_u8(9u, 2u, 10u);
    jp_put_bkg_text(7u, 3u,  "M ");
    put_u8(9u, 3u, 10u);

    jp_put_bkg_text(13u, 1u, "まほう");
    jp_put_bkg_text(13u, 2u, "H ");
    put_u8(15u, 2u, 12u);
    jp_put_bkg_text(13u, 3u, "M ");
    put_u8(15u, 3u, 24u);
}

static void hide_battle_enemy_sprites(void) {
    UINT8 i;

    for (i = 0u; i < 12u; i++) {
        move_sprite(i, 0u, 0u);
    }
}

static void hide_all_sprites_safe(void) {
    UINT8 i;
    for (i = 0u; i < 40u; i++) {
        move_sprite(i, 0u, 0u);
    }
}

static void load_battle_party_icon_sprite_data(void) {
    /* rpg080: load the user-provided party display icons into NPC tile slots.
     * NPC field tiles are restored by init_map_sprites() after battle.
     */
    set_banked_sprite_data(BATTLE_PARTY_ICON_TILE_BASE, 12u, battle_party_display_tiles, BANK(sprite_data_bank));
}

static void show_party_icon_16(UINT8 sprite_base, UINT8 tile_base, UINT8 x, UINT8 y) {
    set_sprite_tile(sprite_base + 0u, tile_base + 0u);
    set_sprite_tile(sprite_base + 1u, tile_base + 1u);
    set_sprite_tile(sprite_base + 2u, tile_base + 2u);
    set_sprite_tile(sprite_base + 3u, tile_base + 3u);

    move_sprite(sprite_base + 0u, (UINT8)(x + 8u),  (UINT8)(y + 16u));
    move_sprite(sprite_base + 1u, (UINT8)(x + 16u), (UINT8)(y + 16u));
    move_sprite(sprite_base + 2u, (UINT8)(x + 8u),  (UINT8)(y + 24u));
    move_sprite(sprite_base + 3u, (UINT8)(x + 16u), (UINT8)(y + 24u));
}

static void show_battle_party_icons(void) {
    load_battle_party_icon_sprite_data();

    /* order shown on screen: ゆうしゃ, そうりょ, まほう.
     * rpg081: y=16 raises the icons by one BG row from rpg080, so the 16x16
     * icon sits beside the H/M numeric area instead of dropping into the
     * bottom border of the party strip.
     */
    show_party_icon_16(BATTLE_PARTY_ICON0_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 4u),  40u, 16u);
    show_party_icon_16(BATTLE_PARTY_ICON1_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 8u),  88u, 16u);
    show_party_icon_16(BATTLE_PARTY_ICON2_SPRITE, (UINT8)(BATTLE_PARTY_ICON_TILE_BASE + 0u), 136u, 16u);
}

static void load_battle_enemy_sprite_data(void) {
    /* rpg077: battle-only load of the first 12 enemy OBJ tiles.
     * Map return still calls init_map_sprites(), so normal field sprite state is
     * restored after battle.  This keeps the new 3-type battle sprites isolated.
     */
    set_banked_sprite_data(ENEMY0_TILE_BASE, 12u, enemy_tiles, BANK(sprite_data_bank));
}

static void show_one_battle_enemy_sprite(UINT8 sprite_base, UINT8 x, UINT8 y, UINT8 sprite_kind) {
    UINT8 tile_base;

    if (sprite_kind > 2u) sprite_kind = 0u;
    tile_base = (UINT8)(ENEMY0_TILE_BASE + (UINT8)(sprite_kind * 4u));

    set_sprite_tile(sprite_base + 0u, tile_base + 0u);
    set_sprite_tile(sprite_base + 1u, tile_base + 1u);
    set_sprite_tile(sprite_base + 2u, tile_base + 2u);
    set_sprite_tile(sprite_base + 3u, tile_base + 3u);

    move_sprite(sprite_base + 0u, (UINT8)(x + 8u),  (UINT8)(y + 16u));
    move_sprite(sprite_base + 1u, (UINT8)(x + 16u), (UINT8)(y + 16u));
    move_sprite(sprite_base + 2u, (UINT8)(x + 8u),  (UINT8)(y + 24u));
    move_sprite(sprite_base + 3u, (UINT8)(x + 16u), (UINT8)(y + 24u));
}

static void show_battle_enemy_sprites(void) {
    hide_battle_enemy_sprites();

    if (battle_enemy_count == 0u) return;

    if (battle_enemy_count == 1u) {
        if (enemy_battles[0].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY0_SPRITE_BASE, 72u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[0]);
        }
    } else if (battle_enemy_count == 2u) {
        if (enemy_battles[0].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY0_SPRITE_BASE, 56u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[0]);
        }
        if (enemy_battles[1].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY1_SPRITE_BASE, 96u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[1]);
        }
    } else {
        if (enemy_battles[0].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY0_SPRITE_BASE, 36u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[0]);
        }
        if (enemy_battles[1].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY1_SPRITE_BASE, 76u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[1]);
        }
        if (enemy_battles[2].hp > 0u) {
            show_one_battle_enemy_sprite(BATTLE_ENEMY2_SPRITE_BASE, 116u, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[2]);
        }
    }
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
    if (battle_enemy_sprite_kinds[slot] > 2u) battle_enemy_sprite_kinds[slot] = 0u;
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

static UINT8 battle_alive_count(void) {
    UINT8 i;
    UINT8 count = 0u;

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp > 0u) count++;
    }
    return count;
}

static void draw_battle_frame(void) {
    battle_clear_bg_full();
    draw_party_status_box();
    draw_battle_enemy_names();
    draw_battle_menu();
    draw_battle_message_area();
}

static void draw_battle_message_area(void) {
    jp_bkg_clear_area(BATTLE_MSG_X, BATTLE_MSG_Y, BATTLE_MSG_W, BATTLE_MSG_H);
}

static void draw_battle_menu(void) {
    /* rpg082:
     * The command text is static BG.  The selected marker is no longer a BG
     * ">" glyph, because rewriting BG text on every cursor move caused
     * visible flicker.  OAM 24 is moved instead.
     */
    draw_bkg_box(9u, 11u, 11u, 4u);

    jp_put_bkg_text(11u, 12u, "こうげき");
    jp_put_bkg_text(16u, 12u, "とくぎ");
    jp_put_bkg_text(11u, 13u, "かいふく");
    jp_put_bkg_text(16u, 13u, "にげる");
}

static void battle_command_cursor_bg_pos(UINT8 index, UINT8 *col, UINT8 *row) {
    switch (index) {
        case CMD_ATTACK: *col = 10u; *row = 12u; break;
        case CMD_SKILL:  *col = 15u; *row = 12u; break;
        case CMD_HEAL:   *col = 10u; *row = 13u; break;
        case CMD_RUN:    *col = 15u; *row = 13u; break;
        default:         *col = 10u; *row = 12u; break;
    }
}

static void battle_move_command_cursor_obj(void) {
    UINT8 col;
    UINT8 row;

    battle_command_cursor_bg_pos(menu_index, &col, &row);
    move_sprite(BATTLE_CURSOR_SPRITE,
                (UINT8)(col * 8u + 8u),
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

static UINT8 battle_enemy_sprite_base_for_index(UINT8 enemy_index) {
    if (enemy_index == 0u) return BATTLE_ENEMY0_SPRITE_BASE;
    if (enemy_index == 1u) return BATTLE_ENEMY1_SPRITE_BASE;
    return BATTLE_ENEMY2_SPRITE_BASE;
}

static UINT8 battle_enemy_x_for_index(UINT8 enemy_index) {
    if (battle_enemy_count == 1u) return 72u;
    if (battle_enemy_count == 2u) {
        if (enemy_index == 0u) return 56u;
        return 96u;
    }
    if (enemy_index == 0u) return 36u;
    if (enemy_index == 1u) return 76u;
    return 116u;
}

static void hide_one_battle_body(UINT8 sprite_base) {
    move_sprite(sprite_base + 0u, 0u, 0u);
    move_sprite(sprite_base + 1u, 0u, 0u);
    move_sprite(sprite_base + 2u, 0u, 0u);
    move_sprite(sprite_base + 3u, 0u, 0u);
}

static void battle_flash_enemy_sprite(UINT8 enemy_index) {
    UINT8 sprite_base;
    UINT8 x;

    if (enemy_index >= battle_enemy_count) return;

    sprite_base = battle_enemy_sprite_base_for_index(enemy_index);
    x = battle_enemy_x_for_index(enemy_index);

    hide_one_battle_body(sprite_base);
    wait_vbl_done();
    wait_vbl_done();

    if (enemy_battles[enemy_index].hp > 0u) {
        show_one_battle_enemy_sprite(sprite_base, x, BATTLE_ENEMY_SPRITE_Y, battle_enemy_sprite_kinds[enemy_index]);
    }
}

static void battle_hide_window_and_reset_scroll(void) {
    HIDE_WIN;
    move_win(JP_WIN_X, 144u);
    move_bkg(0u, 0u);
    SHOW_BKG;
    SHOW_SPRITES;
}

static void battle_update_party_hp_dirty(UINT8 slot) {
    if (slot != 0u) return;
    jp_bkg_clear_area(3u, 2u, 3u, 1u);
    put_u8(3u, 2u, player_battle.hp);
}

static void battle_update_party_mp_dirty(UINT8 slot) {
    if (slot != 0u) return;
    jp_bkg_clear_area(3u, 3u, 3u, 1u);
    put_u8(3u, 3u, 38u);
}

static void battle_set_message_dirty(const char *text) {
    battle_message_text = text;
    battle_dirty_flags |= BATTLE_DIRTY_MESSAGE;
}

static void battle_update_dirty(void) {
    UINT8 flags;

    if (!battle_screen_ready) return;

    flags = battle_dirty_flags;
    battle_dirty_flags = BATTLE_DIRTY_NONE;

    if (flags & BATTLE_DIRTY_PARTY_HP) {
        battle_update_party_hp_dirty(0u);
    }
    if (flags & BATTLE_DIRTY_PARTY_MP) {
        battle_update_party_mp_dirty(0u);
    }
    if (flags & BATTLE_DIRTY_ENEMY_OAM) {
        draw_battle_enemy_names();
        show_battle_enemy_sprites();
    }
    if (flags & BATTLE_DIRTY_MESSAGE) {
        draw_battle_message_area();
        if (battle_message_text != 0) {
            jp_put_bkg_text(1u, 15u, battle_message_text);
        }
    }
    if (flags & BATTLE_DIRTY_CURSOR) {
        battle_move_command_cursor_obj();
    }

    wait_vbl_done();
}

static void battle_show_message(const char *text) {
    battle_set_message_dirty(text);
    battle_update_dirty();
    waitpadup();
    waitpad(J_A);
    waitpadup();
    battle_set_message_dirty("");
    battle_update_dirty();
}

static void battle_enter_render_once(void) {
    /* rpg082:
     * The only full battle BG/OAM construction point.  After this, command
     * movement, messages, HP, and enemy visibility use dirty updates only.
     */
    DISPLAY_OFF;
    battle_screen_ready = 0u;
    battle_dirty_flags = BATTLE_DIRTY_NONE;
    battle_message_text = "";

    hide_all_sprites_safe();
    battle_hide_window_and_reset_scroll();
    jp_init();
    load_battle_enemy_sprite_data();
    load_battle_party_icon_sprite_data();
    set_banked_sprite_data(BATTLE_CURSOR_TILE, 1u, battle_cursor_tiles, BANK(sprite_data_bank));
    set_sprite_tile(BATTLE_CURSOR_SPRITE, BATTLE_CURSOR_TILE);

    draw_battle_frame();
    show_battle_party_icons();
    show_battle_enemy_sprites();
    battle_move_command_cursor_obj();
    battle_hide_window_and_reset_scroll();

    battle_screen_ready = 1u;
    DISPLAY_ON;
    wait_vbl_done();
}

static void update_battle_status(void) {
    battle_dirty_flags |= (UINT8)(BATTLE_DIRTY_PARTY_HP | BATTLE_DIRTY_PARTY_MP | BATTLE_DIRTY_ENEMY_OAM | BATTLE_DIRTY_CURSOR);
    battle_update_dirty();
}

static void init_battle_from_enemy(UINT8 enemy_index) {
    UINT8 i;

    current_enemy_index = enemy_index;
    player_battle.name = "ゆうしゃ";
    player_battle.max_hp = player_max_hp_stat;
    player_battle.hp = player_max_hp_stat;
    player_battle.attack = player_attack_stat;
    player_battle.defense = player_defense_stat;
    player_battle.skill_power = player_skill_power_stat;
    player_battle.heal_power = player_heal_power_stat;
    player_battle.agility = player_agility_stat;

    /* rpg081: field-touch battles also use the 3-body encounter table.
     * The touched actor is still remembered in current_enemy_index so it can be
     * removed from the map after victory.
     */
    battle_data_load_random((UINT8)(enemy_index + 5u), battle_enemy_data_slots, &battle_enemy_count);
    battle_enemy_count = 3u;
    if (battle_enemy_count > BATTLE_MAX_ENEMY_COUNT) battle_enemy_count = BATTLE_MAX_ENEMY_COUNT;

    for (i = 0u; i < battle_enemy_count; i++) {
        battle_copy_enemy_from_data(i);
    }

    battle_target_index = 0u;
    battle_select_first_alive();

    menu_index = 0u;
    skill_uses = 3u;
    heal_uses = 2u;
    battle_state = BSTATE_PLAYER;
}


static void battle_start_effect(void) {
    UINT8 i;

    /*
     * rpg068:
     * Palette flash only.  This avoids new BG tiles, new OBJ sprites, window
     * debug layers, or extra VRAM transfers.
     */
    HIDE_WIN;
    SHOW_BKG;
    SHOW_SPRITES;

    for (i = 0u; i < 3u; i++) {
        BGP_REG = 0x1Bu;
        OBP0_REG = 0x1Bu;
        OBP1_REG = 0x1Bu;
        wait_vbl_done();
        wait_vbl_done();

        BGP_REG = 0xE4u;
        OBP0_REG = 0xE4u;
        OBP1_REG = 0xE4u;
        wait_vbl_done();
        wait_vbl_done();
    }
}

static void enter_battle_screen(void) {
    /* Hide map OAM before the transition so the field player cannot leak into
     * the battle intro.  The battle screen itself is then built exactly once.
     */
    hide_all_sprites_safe();
    battle_start_effect();
    battle_enter_render_once();
    battle_show_message("まものが\nあらわれた！");
}


static void enter_battle(UINT8 enemy_index) {
    game_mode = MODE_BATTLE;
    player_moving = 0u;
    init_battle_from_enemy(enemy_index);
    enter_battle_screen();
}

static void init_random_battle_from_field(void) {
    UINT8 i;

    current_enemy_index = 0xFFu;

    player_battle.name = "ゆうしゃ";
    player_battle.max_hp = player_max_hp_stat;
    player_battle.hp = player_max_hp_stat;
    player_battle.attack = player_attack_stat;
    player_battle.defense = player_defense_stat;
    player_battle.skill_power = player_skill_power_stat;
    player_battle.heal_power = player_heal_power_stat;
    player_battle.agility = player_agility_stat;

    battle_data_load_random(random_u8(), battle_enemy_data_slots, &battle_enemy_count);
    if (battle_enemy_count == 0u) battle_enemy_count = 1u;
    if (battle_enemy_count > BATTLE_MAX_ENEMY_COUNT) battle_enemy_count = BATTLE_MAX_ENEMY_COUNT;

    for (i = 0u; i < battle_enemy_count; i++) {
        battle_copy_enemy_from_data(i);
    }

    battle_target_index = 0u;
    battle_select_first_alive();

    menu_index = 0u;
    skill_uses = 3u;
    heal_uses = 2u;
    battle_state = BSTATE_PLAYER;
}


static void enter_random_battle(void) {
    game_mode = MODE_BATTLE;
    player_moving = 0u;
    init_random_battle_from_field();
    enter_battle_screen();
}

static void apply_battle_growth(void) {
    UINT8 roll;

    /* Small SaGa-like post-battle growth hook. Keep it conservative so the
     * working map/battle loop stays stable while the system is expanded. */
    last_growth_type = GROWTH_NONE;
    roll = (UINT8)(random_u8() % 4u);
    if (roll == 0u && player_max_hp_stat < 99u) {
        player_max_hp_stat++;
        last_growth_type = GROWTH_HP;
    } else if (roll == 1u && player_attack_stat < 30u) {
        player_attack_stat++;
        last_growth_type = GROWTH_ATK;
    } else if (roll == 2u && player_defense_stat < 30u) {
        player_defense_stat++;
        last_growth_type = GROWTH_DEF;
    } else if (player_agility_stat < 30u) {
        player_agility_stat++;
        last_growth_type = GROWTH_AGI;
    }
}

static void show_growth_message(void) {
    if (last_growth_type == GROWTH_HP) {
        dialogue_message("HPが\nあがった！");
    } else if (last_growth_type == GROWTH_ATK) {
        dialogue_message("ちからが\nあがった！");
    } else if (last_growth_type == GROWTH_DEF) {
        dialogue_message("まもりが\nあがった！");
    } else if (last_growth_type == GROWTH_AGI) {
        dialogue_message("すばやさが\nあがった！");
    }
}

static void return_to_map_after_battle(UINT8 won) {
    game_mode = MODE_MAP;
    encounter_grace_steps = RANDOM_ENCOUNTER_GRACE_STEPS;

    if (won) {
        apply_battle_growth();
        show_growth_message();
        if (current_enemy_index < ACTOR_COUNT &&
            actors[current_enemy_index].kind == ACTOR_KIND_ENEMY) {
            actors[current_enemy_index].active = 0u;
            actors[current_enemy_index].solid = 0u;
            set_event_flag(FLAG_ENEMY_DEFEATED);
        }
    }
    restore_field_vram_state();
}

static void player_attack(void) {
    UINT8 dmg;

    if (!battle_select_first_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    dmg = calc_damage(3u, player_battle.attack, enemy_battle.defense);
    if (dmg >= enemy_battle.hp) enemy_battle.hp = 0u;
    else enemy_battle.hp = (UINT8)(enemy_battle.hp - dmg);

    battle_show_message("ゆうしゃの こうげき！\nダメージ！");
    battle_flash_enemy_sprite(battle_target_index);

    if (!battle_select_first_alive()) battle_state = BSTATE_WIN;
    else battle_state = BSTATE_ENEMY;

    update_battle_status();
}


static void player_skill(void) {
    UINT8 dmg;

    if (skill_uses == 0u) {
        battle_show_message("もう つかえない！");
        battle_state = BSTATE_PLAYER;
        return;
    }

    if (!battle_select_first_alive()) {
        battle_state = BSTATE_WIN;
        return;
    }

    skill_uses--;
    dmg = calc_damage(player_battle.skill_power, player_battle.attack, enemy_battle.defense);
    if (dmg >= enemy_battle.hp) enemy_battle.hp = 0u;
    else enemy_battle.hp = (UINT8)(enemy_battle.hp - dmg);

    battle_show_message("とくぎを つかった！\nダメージ！");
    battle_flash_enemy_sprite(battle_target_index);

    if (!battle_select_first_alive()) battle_state = BSTATE_WIN;
    else battle_state = BSTATE_ENEMY;

    update_battle_status();
}


static void player_heal(void) {
    UINT8 before;
    if (heal_uses == 0u) {
        battle_show_message("もう かいふく\nできない！");
        battle_state = BSTATE_PLAYER;
        return;
    }
    heal_uses--;
    before = player_battle.hp;
    player_battle.hp = min_u8((UINT8)(player_battle.hp + player_battle.heal_power), player_battle.max_hp);
    battle_show_message("かいふくした！");
    update_battle_status();
    battle_state = BSTATE_ENEMY;
}

static void player_run(void) {
    UINT8 chance;
    UINT8 roll;
    chance = (UINT8)(40u + player_battle.agility - enemy_battle.agility);
    roll = (UINT8)(random_u8() % 100u);
    if (roll < chance) {
        battle_show_message("うまく\nにげだした！");
        battle_state = BSTATE_ESCAPE;
    } else {
        battle_show_message("にげられない！");
        battle_state = BSTATE_ENEMY;
    }
}

static void enemy_turn(void) {
    UINT8 i;
    UINT8 dmg;

    for (i = 0u; i < battle_enemy_count; i++) {
        if (enemy_battles[i].hp == 0u) continue;

        battle_target_index = i;
        dmg = calc_damage(2u, enemy_battles[i].attack, player_battle.defense);
        if (dmg >= player_battle.hp) player_battle.hp = 0u;
        else player_battle.hp = (UINT8)(player_battle.hp - dmg);

        battle_show_message("まもの の こうげき！\nダメージ！");
        update_battle_status();

        if (player_battle.hp == 0u) {
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

    keys = joypad();
    old_menu_index = menu_index;

    if (keys & J_UP) {
        if (menu_index >= 2u) menu_index = (UINT8)(menu_index - 2u);
        else menu_index = (UINT8)(menu_index + 2u);
        update_battle_menu_cursor(old_menu_index, menu_index);
        waitpadup();
    } else if (keys & J_DOWN) {
        if (menu_index < 2u) menu_index = (UINT8)(menu_index + 2u);
        else menu_index = (UINT8)(menu_index - 2u);
        update_battle_menu_cursor(old_menu_index, menu_index);
        waitpadup();
    } else if (keys & J_LEFT) {
        if (menu_index & 1u) menu_index--;
        else menu_index++;
        update_battle_menu_cursor(old_menu_index, menu_index);
        waitpadup();
    } else if (keys & J_RIGHT) {
        if (menu_index & 1u) menu_index--;
        else menu_index++;
        update_battle_menu_cursor(old_menu_index, menu_index);
        waitpadup();
    } else if (keys & J_A) {
        waitpadup();
        switch (menu_index) {
            case CMD_ATTACK: player_attack(); break;
            case CMD_SKILL:  player_skill(); break;
            case CMD_HEAL:   player_heal(); break;
            case CMD_RUN:    player_run(); break;
            default: break;
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
    last_growth_type = GROWTH_NONE;
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
    camera_tx = 0u;
    camera_ty = 0u;
    camera_px = 0u;
    camera_py = 0u;
    camera_target_px = 0u;
    camera_target_py = 0u;
    player_max_hp_stat = 38u;
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
    game_mode = MODE_MAP;
}

void main(void) {
    BGP_REG = 0xE4u;
    OBP0_REG = 0xE4u;
    OBP1_REG = 0xE4u;

    dialogue_init();
    init_game();
    load_map_mode();

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
                    case BSTATE_ENEMY:
                        enemy_turn();
                        if (battle_state == BSTATE_PLAYER) {
                            battle_dirty_flags |= BATTLE_DIRTY_CURSOR;
                            battle_update_dirty();
                        }
                        break;
                    case BSTATE_WIN:
                        battle_show_message("まものを\nたおした！");
                        return_to_map_after_battle(1u);
                        break;
                    case BSTATE_LOSE:
                        battle_show_message("ゆうしゃは\nたおれた…");
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
        wait_vbl_done();
    }
}
