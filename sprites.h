#ifndef SPRITES_H
#define SPRITES_H

#include <gb/gb.h>

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
 */
#define MAP_GFX_TILE_COUNT 17u

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
extern const unsigned char npc_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char enemy_tiles[ACTOR_TOTAL_TILES * 16u];
extern const unsigned char battle_party_display_tiles[12u * 16u];
extern const unsigned char battle_cursor_tiles[16u];

#endif
