#ifndef SPRITES_H
#define SPRITES_H

#include <gb/gb.h>
#include "battle_data.h"

/*
 * ============================================================================
 * GRAPHICS REGISTRY (rpg030)
 * ============================================================================
 *
 * This header is the first checkpoint before changing graphics.
 *
 * BG tiles:
 *   map_gfx_tiles[] must contain exactly MAP_GFX_TILE_COUNT * 16 bytes.
 *   Tile IDs are MAP_TILE_BASE + local index in main.c.
 *
 * OBJ sprite tiles:
 *   player_tiles/npc_tiles/enemy_tiles are separate OBJ tile sheets.
 *   Do not point set_sprite_tile() at MAP_TILE_BASE + N.
 *
 * Expansion rule:
 *   When adding forest, soft mountain, town signs, pots, chests:
 *     1. Prefer BG tiles/metatiles.
 *     2. Increase MAP_GFX_TILE_COUNT only in an isolated build.
 *     3. Do not change object kind, collision, NPCs, and OAM in the same build.
 *
 * Current stable BG layout:
 *   0 floor
 *   1-4 square wall
 *   5-8 sharp mountain
 *   9-12 town marker
 *   13-16 forest canopy
 * ============================================================================
 */

/* BG tiles rebuilt in base06
 * 0 = floor
 * 1-4 = wall 16x16 metatile: TL, TR, BL, BR
 * 5-8 = sharp mountain 16x16 metatile: TL, TR, BL, BR (existing kind 2 slot)
 * 9-12 = town entrance marker 16x16 metatile: TL, TR, BL, BR
 * 13-16 = forest canopy 16x16 metatile: TL, TR, BL, BR
 * 17-20 = dungeon wall 16x16 metatile: TL, TR, BL, BR
 * 21-24 = dungeon pit/hazard 16x16 metatile: TL, TR, BL, BR
 * 25-28 = dungeon chest 16x16 metatile: TL, TR, BL, BR
 */
#define MAP_GFX_TILE_COUNT 29u

/* Actor sprites
 * 1 frame = 16x16 = 4 tiles.
 * 4 directions x 2 walking frames = 8 frames = 32 tiles total.
 *
 * Important:
 *   ACTOR_TILE_COUNT is only the tile count for a single 16x16 frame.
 *   set_sprite_data() must load ACTOR_TOTAL_TILES for an actor graphics set,
 *   otherwise non-down directions point to unloaded/blank sprite tiles.
 */
#define ACTOR_TILE_COUNT 4u
#define ACTOR_TOTAL_TILES 32u

/* Base06 policy:
 * player_tiles and npc_tiles stay as-is because base05 confirmed them OK.
 * wall BG graphics and wall drawing are rebuilt from scratch.
 * map_gfx_tiles now provides true 16x16 walls as four independent 8x8 tiles,
 * so wall drawing no longer repeats one unstable tile over the whole block.
 */

extern const unsigned char map_gfx_tiles[MAP_GFX_TILE_COUNT * 16u];
extern const unsigned char player_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char mage_field_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char priest_field_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char fourth_party_field_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char npc_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char enemy_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char battle_enemy_tiles[48u * 16u];

/* Battle enemies rendered as BG tiles in battle scene.
 * 3 x 32x32 enemies as OBJ exceed hardware scanline/OAM limits, so the
 * battle enemy sheet is also loaded into BG tile space and stamped into the
 * battle background.  Tile range 80-127 is battle-local and restored by
 * field draw_object_map() after battle.
 */
#define BATTLE_ENEMY_BG_TILE_BASE 80u
#define BATTLE_ENEMY_BG_TILE_COUNT 48u
#define BATTLE_ENEMY_BG_W 4u
#define BATTLE_ENEMY_BG_H 4u
#define BATTLE_ENEMY_BG_Y 6u

void map_load_port_overlay_tiles(UINT8 sea_first_tile, UINT8 vbridge_first_tile, UINT8 hbridge_first_tile) BANKED;
void map_load_pot_overlay_tiles(UINT8 pot_first_tile) BANKED;
void map_load_town_sign_overlay_tiles(UINT8 item_first_tile, UINT8 sword_first_tile) BANKED;
void map_load_town_inn_sign_overlay_tiles(UINT8 inn_first_tile) BANKED;
void battle_enemy_bg_load_tiles(void) BANKED;
void battle_enemy_bg_load_tiles_for_formation(UINT8 count, UINT8 *sprite_kinds, const UINT8 *size_kinds) BANKED;
void battle_enemy_bg_draw_all(UINT8 count, const UINT8 *sprite_kinds, const UINT8 *alive_flags) BANKED;
void battle_enemy_bg_draw_slot(UINT8 count, UINT8 slot, UINT8 sprite_kind, UINT8 alive) BANKED;
void battle_enemy_bg_draw_all_sized(UINT8 count, const UINT8 *sprite_kinds, const UINT8 *size_kinds, const UINT8 *alive_flags) BANKED;
void battle_enemy_bg_draw_slot_sized(UINT8 count, UINT8 slot, UINT8 sprite_kind, UINT8 size_kind, UINT8 alive) BANKED;
void battle_enemy_overlay_tick(UINT8 count, const UINT8 *sprite_kinds, const UINT8 *size_kinds, const UINT8 *alive_flags) BANKED;
extern const unsigned char battle_party_display_tiles[12u * 16u];
extern const unsigned char battle_cursor_tiles[32u];

#endif
