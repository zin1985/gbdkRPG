#pragma bank 11

#include <gb/gb.h>
#include <gb/hardware.h>
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"
#include "party_runtime.h"
#include "ui_icons.h"
#include "sprites.h"
#include "banked_graphics.h"
#include "menu_text_dict.h"

BANKREF_EXTERN(sprite_data_bank)


static UINT8 g_inventory_counts[INVENTORY_ITEM_MAX];
static UINT16 g_inventory_money;
/* rpg184: cached visible item list.  The previous menu scanned the whole
 * inventory every time the cursor moved, then redrew the full window.
 * Keep the list in WRAM and refresh it only when counts change. */
static UINT8 g_inventory_visible_ids[INVENTORY_ITEM_MAX];
static UINT8 g_inventory_visible_count_cached;
static UINT8 g_inventory_visible_cache_valid;
static char inventory_item_effect_message[16];

#define INVENTORY_GRID_ROWS 11u
#define INVENTORY_GRID_COLS 2u
#define INVENTORY_GRID_PAGE_COUNT ((UINT8)(INVENTORY_GRID_ROWS * INVENTORY_GRID_COLS))
#define INVENTORY_NAME_TILE_W 5u
#define INVENTORY_CURSOR_SPRITE 30u
#define INVENTORY_CURSOR_TILE 44u
#define INVENTORY_PAGE_TOP_NONE 0xFFu

#ifndef VBK_TILES
#define VBK_TILES 0u
#endif
#ifndef VBK_ATTRIBUTES
#define VBK_ATTRIBUTES 1u
#endif

static UINT8 g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
static UINT8 g_inventory_page_name_tiles[INVENTORY_GRID_PAGE_COUNT][INVENTORY_NAME_TILE_W];
static UINT8 g_inventory_cursor_obj_ready;
/* rpg270: BG Y base for item-page slide. Keep the completed page in the
 * scrolled BG map instead of redrawing it after the animation. */
static UINT8 g_inventory_bgy_base;

#define INVENTORY_SCREEN_CACHE_TILES ((UINT16)JP_BKG_BUFFER_W * (UINT16)JP_BKG_BUFFER_H)
static UINT8 g_inventory_screen_cache[JP_BKG_BUFFER_W * JP_BKG_BUFFER_H];
static UINT8 g_inventory_screen_cache_valid;
static UINT8 g_inventory_screen_cache_top = INVENTORY_PAGE_TOP_NONE;
/* rpg272: FF4-style row crawl cache.  Keep the window/frame fixed and
 * update only the 11 item rows.  Each row is 18 tiles wide (x=1..18),
 * so a down/up crawl can reuse 10 cached rows and build only one new row. */
#define INVENTORY_ROW_TILE_W 18u
#define INVENTORY_ROW_CACHE_NONE 0xFFu
static UINT8 g_inventory_row_tiles[INVENTORY_GRID_ROWS][INVENTORY_ROW_TILE_W];
static UINT8 g_inventory_row_prefetch_prev[INVENTORY_ROW_TILE_W];
static UINT8 g_inventory_row_prefetch_next[INVENTORY_ROW_TILE_W];
static UINT8 g_inventory_row_scratch[20u];
static UINT8 g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
/* rpg278: CGB fixed name glyph area.
 * Item/equipment names in the list are rendered into VRAM bank 1 fixed slots,
 * so they no longer depend on jpfont's dynamic cache used by title/footer text.
 */
#define INVENTORY_NAME_FIXED_BASE 0u
#define INVENTORY_NAME_FIXED_W INVENTORY_NAME_TILE_W
#define INVENTORY_NAME_FIXED_SLOTS INVENTORY_GRID_PAGE_COUNT
#define INVENTORY_NAME_FIXED_BANK_ATTR 0x08u
static UINT8 g_inventory_row_ring_base;
#define INVENTORY_MARK_NONE 0xFFu
#define INVENTORY_SORT_ID 0u
#define INVENTORY_SORT_KIND 1u
#define INVENTORY_SORT_KANA 2u
#define INVENTORY_SORT_FIELD 3u
#define INVENTORY_SORT_COUNT 4u
#define INVENTORY_SORT_MANUAL 5u
static UINT8 g_inventory_mark_index = INVENTORY_MARK_NONE;
static UINT8 g_inventory_sort_mode = INVENTORY_SORT_ID;
#define g_inventory_default_msg menu_dict_message(MENU_DICT_MSG_ITEM_HELP)


static UINT8 inventory_visible_count(void) BANKED;
static UINT8 inventory_item_at_visible_index(UINT8 index) BANKED;
static UINT8 inventory_clamp_page_top(UINT8 page_top, UINT8 visible_count) BANKED;
static void inventory_draw_items_page(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED;
static void inventory_draw_full_screen_buffered(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED;
static void inventory_draw_full_screen_direct(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED;
static void inventory_draw_menu_shell(void) BANKED;
static void inventory_clear_list_area(void) BANKED;
static void inventory_draw_cursor_only(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED;
static void inventory_menu_attr_rect(UINT8 x, UINT8 y, UINT8 w, UINT8 h, UINT8 attr) BANKED;
static void inventory_cursor_obj_prepare(void) BANKED;
static void inventory_cursor_obj_hide(void) BANKED;
static void inventory_cursor_obj_move(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED;
static void inventory_hide_all_oam_slots(void) BANKED;
static void inventory_prepare_page_name_cache(UINT8 page_top) BANKED;
static void inventory_put_cached_name(UINT8 slot, UINT8 x, UINT8 y) BANKED;
static void inventory_fixed_name_load_slot(UINT8 slot, UINT8 item_id);
static void inventory_fixed_name_put_slot(UINT8 slot, UINT8 x, UINT8 y);
static void inventory_apply_visible_name_attrs(UINT8 page_top);
static void inventory_clear_name_cells_for_screen_row(UINT8 y);
static UINT8 inventory_physical_row_for_display(UINT8 display_row);
static void inventory_update_quantity_only(UINT8 visible_index, UINT8 page_top) BANKED;
static void inventory_put_count(UINT8 x, UINT8 y, UINT8 value) BANKED;
static void inventory_draw_page_counter(UINT8 cursor_index, UINT8 visible_count) BANKED;
static void inventory_slide_page_transition(UINT8 cursor_index, UINT8 old_page_top, UINT8 new_page_top, const char *message);
static void inventory_reset_scroll_base(void);
static void inventory_normalize_after_slide(UINT8 cursor_index, UINT8 page_top, const char *message);
static void inventory_redraw_list_for_row_scroll(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED;
static void inventory_protect_footer_message(const char *message) BANKED;
static void inventory_invalidate_screen_cache(void);
static void inventory_build_screen_cache(UINT8 page_top, const char *message);
static UINT8 inventory_commit_screen_cache(UINT8 cursor_index, UINT8 page_top);
static void inventory_precache_next_page(UINT8 page_top, const char *message);
static void inventory_build_row_tiles(UINT8 row_cache_index, UINT8 first_visible_index);
static void inventory_build_visible_rows_cache(UINT8 page_top);
static void inventory_put_cached_row(UINT8 row_cache_index, UINT8 y);
static void inventory_commit_list_rows_cache(void);
static void inventory_scroll_list_cache_to(UINT8 new_page_top);
static void inventory_build_prefetch_rows(UINT8 page_top);
static void inventory_copy_row(UINT8 *dst, const UINT8 *src);
static void inventory_draw_mark(UINT8 visible_index, UINT8 page_top, UINT8 visible) BANKED;
static void inventory_swap_visible_entries(UINT8 a, UINT8 b) BANKED;
static void inventory_show_sort_popup(UINT8 *cursor_index, UINT8 *page_top, const char **message) BANKED;
static void inventory_apply_sort_to_visible_cache(void) BANKED;
static UINT8 inventory_cursor_down(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED;
static UINT8 inventory_cursor_up(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED;
static UINT8 inventory_wait_menu_keys(UINT8 mask) BANKED;
static const char *inventory_make_item_result_message(UINT8 item_id) BANKED;
static UINT8 inventory_effect_is_growth(UINT8 item_id) BANKED;
static void inventory_draw_target_popup(UINT8 item_id, UINT8 slot_cursor) BANKED;
static UINT8 inventory_is_field_usable(UINT8 item_id) BANKED;

void inventory_clear(void) BANKED {
    UINT8 i;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        g_inventory_counts[i] = 0u;
    }
    g_inventory_money = 0u;
    g_inventory_visible_cache_valid = 0u;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    g_inventory_mark_index = INVENTORY_MARK_NONE;
    inventory_invalidate_screen_cache();
    g_inventory_bgy_base = 0u;
    g_inventory_row_ring_base = 0u;
}


void inventory_seed_defaults(void) BANKED {
    inventory_clear();
    inventory_add(ITEM_HERB, 6u);
    inventory_add(ITEM_POTION, 4u);
    inventory_add(ITEM_MANA_HERB, 2u);
    inventory_add(ITEM_ANTIDOTE, 2u);
    inventory_add(ITEM_CASTLE_KEY, 1u);
    inventory_add(ITEM_WOOD_SWORD, 1u);
    inventory_add(ITEM_SHORT_BOW, 1u);
    inventory_add(ITEM_TOOL_KIT, 1u);
    inventory_add(ITEM_CLOTH_ARMOR, 1u);
    inventory_add(ITEM_CHARM, 1u);
    inventory_add(ITEM_DEBUG_NO_ENCOUNT, 1u);
    inventory_add(ITEM_DEBUG_ESCAPE, 1u);

    /* rpg182: seed enough item kinds to verify 2-column scrolling on real hardware. */
    inventory_add(ITEM_MEDICINE, 5u);
    inventory_add(ITEM_HIGH_POTION, 3u);
    inventory_add(ITEM_NAGAI_TEA, 3u);
    inventory_add(ITEM_ELIXIR, 1u);
    inventory_add(ITEM_MANA_DROP, 5u);
    inventory_add(ITEM_MANA_BOTTLE, 3u);
    inventory_add(ITEM_FULL_HERB, 2u);
    inventory_add(ITEM_PANACEA, 2u);
    inventory_add(ITEM_WAKE_HERB, 2u);
    inventory_add(ITEM_EYE_DROP, 2u);
    inventory_add(ITEM_SMOKE_BOMB, 3u);
    inventory_add(ITEM_BARRIER_SEED, 2u);
    inventory_add(ITEM_POWER_SEED, 2u);
    inventory_add(ITEM_GUARD_SEED, 2u);
    inventory_add(ITEM_SPEED_SEED, 2u);
    inventory_add(ITEM_FOCUS_TEA, 2u);
    inventory_add(ITEM_MORALE_MEAT, 2u);
    inventory_add(ITEM_ICE_BOMB, 3u);
    inventory_add(ITEM_FIRE_BOMB, 3u);
    inventory_add(ITEM_THUNDER_BOMB, 3u);
    inventory_add(ITEM_LIGHT_BOTTLE, 2u);
    inventory_add(ITEM_DARK_BOTTLE, 2u);
    inventory_add(ITEM_RETURN_FEATHER, 1u);
    inventory_add(ITEM_TENT, 1u);
    inventory_add(ITEM_REVIVE_STONE, 1u);

    inventory_add(ITEM_COPPER_SWORD, 1u);
    inventory_add(ITEM_STEEL_SWORD, 1u);
    inventory_add(ITEM_SILVER_SWORD, 1u);
    inventory_add(ITEM_FLAME_SWORD, 1u);
    inventory_add(ITEM_ICE_SWORD, 1u);
    inventory_add(ITEM_WILLOW_STAFF, 1u);
    inventory_add(ITEM_SAGE_STAFF, 1u);
    inventory_add(ITEM_LONG_BOW, 1u);
    inventory_add(ITEM_STEEL_BOW, 1u);
    inventory_add(ITEM_BRONZE_SPEAR, 1u);
    inventory_add(ITEM_STEEL_SPEAR, 1u);
    inventory_add(ITEM_HAND_AXE, 1u);
    inventory_add(ITEM_BATTLE_AXE, 1u);
    inventory_add(ITEM_IRON_GLOVES, 1u);
    inventory_add(ITEM_BEAST_CLAWS, 1u);
    inventory_add(ITEM_POWER_KNUCKLE, 1u);
    inventory_add(ITEM_ALCHEMY_KIT, 1u);
    inventory_add(ITEM_TRAVEL_CLOTH, 1u);
    inventory_add(ITEM_MAGE_ROBE, 1u);
    inventory_add(ITEM_CHAIN_MAIL, 1u);
    inventory_add(ITEM_STEEL_ARMOR, 1u);
    inventory_add(ITEM_QUICK_BOOTS, 1u);
    inventory_add(ITEM_POWER_RING, 1u);
    inventory_add(ITEM_GUARD_RING, 1u);
    inventory_add(ITEM_FOCUS_RING, 1u);
    inventory_add(ITEM_POISON_GUARD, 1u);
    g_inventory_money = 120u;
}

UINT16 inventory_get_money(void) BANKED {
    return g_inventory_money;
}

void inventory_add_money(UINT16 amount) BANKED {
    if ((UINT16)(9999u - g_inventory_money) < amount) {
        g_inventory_money = 9999u;
    } else {
        g_inventory_money = (UINT16)(g_inventory_money + amount);
    }
}

void inventory_set_money(UINT16 amount) BANKED {
    g_inventory_money = (amount > 9999u) ? 9999u : amount;
}

UINT8 inventory_spend_money(UINT16 amount) BANKED {
    if (g_inventory_money < amount) return 0u;
    g_inventory_money = (UINT16)(g_inventory_money - amount);
    return 1u;
}

UINT8 inventory_get_count(UINT8 item_id) BANKED {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return g_inventory_counts[item_id];
}

UINT8 inventory_has(UINT8 item_id, UINT8 amount) BANKED {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return (g_inventory_counts[item_id] >= amount) ? 1u : 0u;
}

UINT8 inventory_add(UINT8 item_id, UINT8 amount) BANKED {
    UINT16 total;

    if (item_id == ITEM_NONE) return 0u;
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;

    total = (UINT16)g_inventory_counts[item_id] + (UINT16)amount;
    if (total > INVENTORY_STACK_MAX) {
        g_inventory_counts[item_id] = INVENTORY_STACK_MAX;
        g_inventory_visible_cache_valid = 0u;
        g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
        inventory_invalidate_screen_cache();
        return 0u;
    }

    g_inventory_counts[item_id] = (UINT8)total;
    g_inventory_visible_cache_valid = 0u;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    inventory_invalidate_screen_cache();
    return 1u;
}

UINT8 inventory_remove(UINT8 item_id, UINT8 amount) BANKED {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;
    if (g_inventory_counts[item_id] < amount) return 0u;

    g_inventory_counts[item_id] = (UINT8)(g_inventory_counts[item_id] - amount);
    g_inventory_visible_cache_valid = 0u;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    g_inventory_mark_index = INVENTORY_MARK_NONE;
    inventory_invalidate_screen_cache();
    return 1u;
}

void inventory_copy_to(UINT8 *dst) BANKED {
    UINT8 i;

    if (dst == 0) return;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        dst[i] = g_inventory_counts[i];
    }
}

void inventory_copy_from(const UINT8 *src) BANKED {
    UINT8 i;

    if (src == 0) return;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        if (src[i] > INVENTORY_STACK_MAX) {
            g_inventory_counts[i] = INVENTORY_STACK_MAX;
        } else {
            g_inventory_counts[i] = src[i];
        }
    }
    g_inventory_visible_cache_valid = 0u;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    inventory_invalidate_screen_cache();
}



static void inventory_battle_leave_screen(void) BANKED {
    /* Hide the item menu before returning to the battle renderer.
     * The menu uses its own BG tile cache / fixed item-name tiles; on slow
     * devices the old BG can be visible for a moment and looks like many
     * stray numbers.  The battle renderer immediately rebuilds the screen.
     */
    HIDE_BKG;
    HIDE_SPRITES;
    DISPLAY_OFF;
}

UINT8 inventory_battle_use_auto(UINT8 active_slot) BANKED {
    UINT8 item_id;
    item_id = ITEM_NONE;
    if (inventory_has(ITEM_HERB, 1u)) item_id = ITEM_HERB;
    else if (inventory_has(ITEM_POTION, 1u)) item_id = ITEM_POTION;
    else if (inventory_has(ITEM_NAGAI_TEA, 1u)) item_id = ITEM_NAGAI_TEA;
    else if (inventory_has(ITEM_MANA_HERB, 1u)) item_id = ITEM_MANA_HERB;
    else if (inventory_has(ITEM_ANTIDOTE, 1u)) item_id = ITEM_ANTIDOTE;
    if (item_id == ITEM_NONE) return 0u;
    if (party_use_field_item_on_active(item_id, active_slot) == 0u) return 1u;
    inventory_remove(item_id, 1u);
    return 2u;
}


static UINT8 inventory_is_battle_usable(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HERB:
        case ITEM_POTION:
        case ITEM_MANA_HERB:
        case ITEM_ANTIDOTE:
        case ITEM_MEDICINE:
        case ITEM_HIGH_POTION:
        case ITEM_NAGAI_TEA:
        case ITEM_ELIXIR:
        case ITEM_MANA_DROP:
        case ITEM_MANA_BOTTLE:
        case ITEM_FULL_HERB:
        case ITEM_PANACEA:
        case ITEM_WAKE_HERB:
        case ITEM_EYE_DROP:
        case ITEM_BARRIER_SEED:
        case ITEM_POWER_SEED:
        case ITEM_GUARD_SEED:
        case ITEM_SPEED_SEED:
        case ITEM_FOCUS_TEA:
        case ITEM_MORALE_MEAT:
        case ITEM_REVIVE_STONE:
            return 1u;
        default:
            return 0u;
    }
}

static void inventory_rebuild_battle_visible_cache(void) BANKED {
    UINT8 id;
    UINT8 count;

    count = 0u;
    for (id = 1u; id < INVENTORY_ITEM_MAX; id++) {
        if (g_inventory_counts[id] == 0u) continue;
        if (!inventory_is_battle_usable(id)) continue;
        g_inventory_visible_ids[count] = id;
        count++;
    }
    g_inventory_visible_count_cached = count;
}

UINT8 inventory_battle_select_use(UINT8 active_slot) BANKED {
    UINT8 keys;
    UINT8 cursor_index;
    UINT8 page_top;
    UINT8 visible_count;
    UINT8 old_cursor;
    UINT8 old_page;
    UINT8 item_id;
    UINT8 result;
    UINT8 target_slot;
    UINT8 popup_redraw;
    const char *msg;

    ui_icons_load();
    inventory_hide_all_oam_slots();
    HIDE_SPRITES;
    inventory_rebuild_battle_visible_cache();
    visible_count = inventory_visible_count();
    cursor_index = 0u;
    page_top = 0u;
    target_slot = active_slot;
    msg = menu_dict_message(MENU_DICT_MSG_ITEM_HELP_KANJI);
    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);

    while (1) {
        visible_count = inventory_visible_count();
        if (visible_count == 0u) {
            inventory_draw_full_screen_buffered(0u, 0u, menu_dict_message(MENU_DICT_MSG_EMPTY_USABLE));
            keys = inventory_wait_menu_keys(J_B | J_START | J_A);
            (void)keys;
            inventory_battle_leave_screen();
            return 0u;
        }
        if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
        page_top = inventory_clamp_page_top(page_top, visible_count);
        old_cursor = cursor_index;
        old_page = page_top;

        keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START | J_SELECT);
        if (keys & (J_B | J_START)) {
            inventory_battle_leave_screen();
            return 0u;
        }
        if (keys & J_UP) cursor_index = inventory_cursor_up(cursor_index, &page_top, visible_count);
        else if (keys & J_DOWN) cursor_index = inventory_cursor_down(cursor_index, &page_top, visible_count);
        else if (keys & J_LEFT) {
            if ((cursor_index & 1u) && cursor_index > page_top) cursor_index--;
        } else if (keys & J_RIGHT) {
            if (((cursor_index & 1u) == 0u) && ((UINT8)(cursor_index + 1u) < visible_count) && ((UINT8)(cursor_index + 1u) < (UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT))) cursor_index++;
        } else if (keys & J_A) {
            item_id = inventory_item_at_visible_index(cursor_index);
            inventory_draw_items_page(cursor_index, page_top, menu_dict_message(MENU_DICT_MSG_TARGET));
            inventory_draw_target_popup(item_id, target_slot);
            while (1) {
                keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_A | J_B);
                popup_redraw = 0u;
                if (keys & J_UP) {
                    if (target_slot == 0u) target_slot = PARTY_ACTIVE_COUNT - 1u;
                    else target_slot--;
                    popup_redraw = 1u;
                } else if (keys & J_DOWN) {
                    target_slot++;
                    if (target_slot >= PARTY_ACTIVE_COUNT) target_slot = 0u;
                    popup_redraw = 1u;
                } else if (keys & J_B) {
                    msg = menu_dict_message(MENU_DICT_MSG_ITEM_HELP_KANJI);
                    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);
                    break;
                } else if (keys & J_A) {
                    result = party_use_field_item_on_active(item_id, target_slot);
                    if (result != 0u) {
                        inventory_remove(item_id, 1u);
                        msg = inventory_make_item_result_message(item_id);
                        (void)msg;
                        inventory_battle_leave_screen();
                        return 2u;
                    }
                    msg = menu_dict_message(MENU_DICT_MSG_NO_EFFECT);
                    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);
                    break;
                }
                if (popup_redraw != 0u) inventory_draw_target_popup(item_id, target_slot);
            }
            continue;
        }

        if (old_cursor != cursor_index || old_page != page_top) {
            if (old_page != page_top) inventory_redraw_list_for_row_scroll(cursor_index, page_top, msg);
            else {
                /* rpg270: cursor movement after slide is OBJ-only.
                 * Keep the completed BG map in place and avoid hidden redraws. */
                inventory_draw_cursor_only(old_cursor, page_top, 0u);
                inventory_draw_cursor_only(cursor_index, page_top, 1u);
            }
        }
    }
}

static const char *inventory_item_name(UINT8 item_id) BANKED {
    return menu_dict_item_name(item_id);
}


const char *inventory_get_item_name(UINT8 item_id) BANKED {
    return menu_dict_item_name(item_id);
}
static const char *inventory_item_short_name(UINT8 item_id) BANKED {
    return menu_dict_item_short_name(item_id);
}


static void inventory_fixed_rotate_right(const byte *src, byte *dst) {
    UINT8 y;
    UINT8 x;
    byte row;

    for (y = 0u; y < 8u; y++) dst[y] = 0u;
    for (y = 0u; y < 8u; y++) {
        row = src[y];
        for (x = 0u; x < 8u; x++) {
            if (row & (1u << x)) {
                dst[x] |= (byte)(1u << (7u - y));
            }
        }
    }
}

static void inventory_fixed_font_to_tile(const byte *src, unsigned char *dst) {
    byte rotated[8];
    UINT8 y;

    inventory_fixed_rotate_right(src, rotated);
    for (y = 0u; y < 8u; y++) {
        dst[y * 2u] = rotated[y];
        dst[y * 2u + 1u] = rotated[y];
    }
}

static void inventory_fixed_load_tile(UINT8 tile_id, const unsigned char *tile) {
    VBK_REG = VBK_ATTRIBUTES;
    set_bkg_data(tile_id, 1u, tile);
    VBK_REG = VBK_TILES;
}

static void inventory_fixed_load_blank_tile(UINT8 tile_id) {
    static const unsigned char blank_tile[16] = {
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0
    };
    inventory_fixed_load_tile(tile_id, blank_tile);
}

static const char *inventory_fixed_load_utf8_tile(UINT8 tile_id, const char *p) {
    byte font8[8];
    unsigned char tile[16];
    const char *next;

    if (p == 0 || *p == '\0') {
        inventory_fixed_load_blank_tile(tile_id);
        return p;
    }

    next = getFontData(font8, p, 1);
    if (next == 0 || next == p) {
        inventory_fixed_load_blank_tile(tile_id);
        return (const char *)(p + 1);
    }

    inventory_fixed_font_to_tile(font8, tile);
    inventory_fixed_load_tile(tile_id, tile);
    return next;
}

static void inventory_fixed_name_load_slot(UINT8 slot, UINT8 item_id) {
    UINT8 i;
    UINT8 tile_id;
    const char *p;

    if (slot >= INVENTORY_NAME_FIXED_SLOTS) return;
    tile_id = (UINT8)(INVENTORY_NAME_FIXED_BASE + (slot * INVENTORY_NAME_FIXED_W));
    p = (item_id == ITEM_NONE) ? 0 : inventory_item_short_name(item_id);

    for (i = 0u; i < INVENTORY_NAME_FIXED_W; i++) {
        if (p != 0 && *p != '\0') p = inventory_fixed_load_utf8_tile((UINT8)(tile_id + i), p);
        else inventory_fixed_load_blank_tile((UINT8)(tile_id + i));
    }
}

static void inventory_fixed_name_put_slot(UINT8 slot, UINT8 x, UINT8 y) {
    UINT8 i;
    UINT8 tile_id;
    UINT16 base;
    UINT8 row_tiles[INVENTORY_NAME_FIXED_W];

    if (slot >= INVENTORY_NAME_FIXED_SLOTS) return;
    tile_id = (UINT8)(INVENTORY_NAME_FIXED_BASE + (slot * INVENTORY_NAME_FIXED_W));
    if (jp_bkg_buffer_active) {
        base = ((UINT16)y * JP_BKG_BUFFER_W) + x;
        for (i = 0u; i < INVENTORY_NAME_FIXED_W; i++) {
            jp_bkg_backbuffer[base + i] = (UINT8)(tile_id + i);
        }
    } else {
        for (i = 0u; i < INVENTORY_NAME_FIXED_W; i++) row_tiles[i] = (UINT8)(tile_id + i);
        set_bkg_tiles(x, y, INVENTORY_NAME_FIXED_W, 1u, row_tiles);
    }
}

static UINT8 inventory_physical_row_for_display(UINT8 display_row) {
    UINT8 row;
    row = (UINT8)(g_inventory_row_ring_base + display_row);
    while (row >= INVENTORY_GRID_ROWS) row = (UINT8)(row - INVENTORY_GRID_ROWS);
    return row;
}

static void inventory_apply_name_attr_cells(UINT8 x, UINT8 y) {
    static UINT8 attr_row[INVENTORY_NAME_FIXED_W];
    UINT8 i;
    for (i = 0u; i < INVENTORY_NAME_FIXED_W; i++) attr_row[i] = INVENTORY_NAME_FIXED_BANK_ATTR;
    VBK_REG = VBK_ATTRIBUTES;
    set_bkg_tiles(x, y, INVENTORY_NAME_FIXED_W, 1u, attr_row);
    VBK_REG = VBK_TILES;
}

static void inventory_clear_name_cells_for_screen_row(UINT8 y) {
    static UINT8 blank_name[INVENTORY_NAME_FIXED_W];
    static UINT8 attr_name[INVENTORY_NAME_FIXED_W];
    UINT8 i;

    for (i = 0u; i < INVENTORY_NAME_FIXED_W; i++) {
        blank_name[i] = JP_FRAME_BASE + 0u;
        attr_name[i] = 0u;
    }

    /* Clear both name cells before reusing their fixed VRAM slots.
     * Otherwise changing the slot glyphs can be visible for one frame on
     * the outgoing row.  Keep icons/counts untouched so the crawl remains calm.
     */
    VBK_REG = VBK_TILES;
    set_bkg_tiles(3u, y, INVENTORY_NAME_FIXED_W, 1u, blank_name);
    set_bkg_tiles(12u, y, INVENTORY_NAME_FIXED_W, 1u, blank_name);
    VBK_REG = VBK_ATTRIBUTES;
    set_bkg_tiles(3u, y, INVENTORY_NAME_FIXED_W, 1u, attr_name);
    set_bkg_tiles(12u, y, INVENTORY_NAME_FIXED_W, 1u, attr_name);
    VBK_REG = VBK_TILES;
}

static void inventory_apply_visible_name_attrs(UINT8 page_top) {
    UINT8 row;
    UINT8 idx;
    UINT8 y;
    UINT8 count;

    count = inventory_visible_count();
    if (page_top == INVENTORY_ROW_CACHE_NONE) return;
    page_top = inventory_clamp_page_top(page_top, count);
    y = 3u;
    idx = page_top;
    for (row = 0u; row < INVENTORY_GRID_ROWS; row++, y++, idx = (UINT8)(idx + 2u)) {
        if (idx < count && inventory_item_at_visible_index(idx) != ITEM_NONE) inventory_apply_name_attr_cells(3u, y);
        if ((UINT8)(idx + 1u) < count && inventory_item_at_visible_index((UINT8)(idx + 1u)) != ITEM_NONE) inventory_apply_name_attr_cells(12u, y);
    }
}





static void inventory_menu_attr_rect(UINT8 x, UINT8 y, UINT8 w, UINT8 h, UINT8 attr) BANKED {
    UINT8 xx;
    UINT8 yy;
    static UINT8 attr_row[20u];

    if (w == 0u || h == 0u) return;
    if (w > 20u) w = 20u;
    attr &= 0x07u;
    for (xx = 0u; xx < w; xx++) attr_row[xx] = attr;

    VBK_REG = VBK_ATTRIBUTES;
    for (yy = 0u; yy < h; yy++) {
        set_bkg_tiles(x, (UINT8)(y + yy), w, 1u, attr_row);
    }
    VBK_REG = VBK_TILES;
}

static void inventory_invalidate_screen_cache(void) {
    g_inventory_screen_cache_valid = 0u;
    g_inventory_screen_cache_top = INVENTORY_PAGE_TOP_NONE;
    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
}

static void inventory_copy_backbuffer_to_cache(void) {
    UINT16 i;
    for (i = 0u; i < INVENTORY_SCREEN_CACHE_TILES; i++) {
        g_inventory_screen_cache[i] = jp_bkg_backbuffer[i];
    }
}

static void inventory_build_screen_cache(UINT8 page_top, const char *message) {
    UINT8 prev_active;

    inventory_prepare_page_name_cache(page_top);
    prev_active = jp_bkg_buffer_active;
    jp_bkg_buffer_begin();
    inventory_draw_menu_shell();
    inventory_draw_items_page(0u, page_top, message);
    inventory_copy_backbuffer_to_cache();
    jp_bkg_buffer_active = prev_active;
    g_inventory_screen_cache_top = page_top;
    g_inventory_screen_cache_valid = 1u;
}

static UINT8 inventory_commit_screen_cache(UINT8 cursor_index, UINT8 page_top) {
    if (g_inventory_screen_cache_valid == 0u || g_inventory_screen_cache_top != page_top) return 0u;
    DISPLAY_OFF;
    set_bkg_tiles(0u, 0u, JP_BKG_BUFFER_W, JP_BKG_BUFFER_H, g_inventory_screen_cache);
    DISPLAY_ON;
    inventory_menu_attr_rect(0u, 0u, 20u, 18u, 0u);
    inventory_apply_visible_name_attrs(page_top);
    inventory_cursor_obj_prepare();
    inventory_cursor_obj_move(cursor_index, page_top, (UINT8)(inventory_visible_count() != 0u));
    inventory_draw_page_counter(cursor_index, inventory_visible_count());
    return 1u;
}

static void inventory_precache_next_page(UINT8 page_top, const char *message) {
    /* rpg266: keep this as a logical warm-up hook only.
     * Drawing next/prev Japanese pages off-screen can evict glyph tiles used by
     * the current screen.  The visible page switch is still buffered, and
     * cursor movement / quantity updates are differential.
     */
    (void)page_top;
    (void)message;
    /* rpg290: do not invalidate the row cache here.  Invalidating after the
     * initial draw made the first next-page crawl rebuild all 11 rows, causing
     * a heavy first scroll.  Counts/sort changes already invalidate explicitly.
     */
}


static void inventory_hide_all_oam_slots(void) BANKED {
    UINT8 i;
    for (i = 0u; i < 40u; i++) {
        move_sprite(i, 0u, 0u);
    }
}

static void inventory_cursor_obj_prepare(void) BANKED {
    if (g_inventory_cursor_obj_ready == 0u) {
        set_banked_sprite_data(INVENTORY_CURSOR_TILE, 2u, battle_cursor_tiles, BANK(sprite_data_bank));
        set_sprite_tile(INVENTORY_CURSOR_SPRITE, INVENTORY_CURSOR_TILE);
        set_sprite_prop(INVENTORY_CURSOR_SPRITE, 0u);
        g_inventory_cursor_obj_ready = 1u;
    }
    SPRITES_8x8;
    SHOW_SPRITES;
}

static void inventory_cursor_obj_hide(void) BANKED {
    move_sprite(INVENTORY_CURSOR_SPRITE, 0u, 0u);
}

static void inventory_cursor_obj_move(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED {
    UINT8 offset;
    UINT8 x;
    UINT8 y;

    if (enabled == 0u || visible_index < page_top) {
        inventory_cursor_obj_hide();
        return;
    }
    offset = (UINT8)(visible_index - page_top);
    if (offset >= INVENTORY_GRID_PAGE_COUNT) {
        inventory_cursor_obj_hide();
        return;
    }
    x = (offset & 1u) ? 10u : 1u;
    y = (UINT8)(3u + (offset >> 1));
    move_sprite(INVENTORY_CURSOR_SPRITE, (UINT8)(x * 8u + 8u), (UINT8)(y * 8u + 16u));
}

static void inventory_cache_name_slot(UINT8 slot, UINT8 item_id) BANKED {
    UINT8 i;
    UINT8 prev_active;
    if (slot >= INVENTORY_GRID_PAGE_COUNT) return;
    for (i = 0u; i < INVENTORY_NAME_TILE_W; i++) {
        g_inventory_page_name_tiles[slot][i] = JP_FRAME_BASE + 0u;
        jp_bkg_backbuffer[i] = JP_FRAME_BASE + 0u;
    }
    if (item_id == ITEM_NONE) return;

    prev_active = jp_bkg_buffer_active;
    jp_bkg_buffer_active = 1u;
    jp_put_bkg_text(0u, 0u, inventory_item_short_name(item_id));
    jp_bkg_buffer_active = prev_active;

    for (i = 0u; i < INVENTORY_NAME_TILE_W; i++) {
        g_inventory_page_name_tiles[slot][i] = jp_bkg_backbuffer[i];
    }
}

static void inventory_prepare_page_name_cache(UINT8 page_top) BANKED {
    /* rpg278: the visible item list no longer uses jpfont dynamic name tiles.
     * Keep this hook as a no-op so old precache paths do not evict footer/title glyphs.
     */
    (void)page_top;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
}

static void inventory_put_cached_name(UINT8 slot, UINT8 x, UINT8 y) BANKED {
    UINT8 i;
    UINT16 base;

    if (slot >= INVENTORY_GRID_PAGE_COUNT) return;
    if (jp_bkg_buffer_active) {
        base = ((UINT16)y * JP_BKG_BUFFER_W) + x;
        for (i = 0u; i < INVENTORY_NAME_TILE_W; i++) {
            jp_bkg_backbuffer[base + i] = g_inventory_page_name_tiles[slot][i];
        }
    } else {
        set_bkg_tiles(x, y, INVENTORY_NAME_TILE_W, 1u, g_inventory_page_name_tiles[slot]);
    }
}

static void inventory_update_quantity_only(UINT8 visible_index, UINT8 page_top) BANKED {
    UINT8 offset;
    UINT8 id;
    UINT8 x;
    UINT8 y;

    if (visible_index < page_top) return;
    offset = (UINT8)(visible_index - page_top);
    if (offset >= INVENTORY_GRID_PAGE_COUNT) return;
    id = inventory_item_at_visible_index(visible_index);
    if (id == ITEM_NONE) return;
    x = (offset & 1u) ? 10u : 1u;
    y = (UINT8)(3u + (offset >> 1));
    inventory_put_count((UINT8)(x + 7u), y, g_inventory_counts[id]);
    /* Count text changed.  Rebuild row-cache lazily before the next crawl so
     * scrolling away and back does not resurrect the old quantity. */
    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
}

static void inventory_ui_clear(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
}

/* rpg238: keep the frame and redraw only the list/message area.
 * Full frame redraw was the main reason item pages felt slow.
 */
static void inventory_clear_list_area(void) BANKED {
    jp_bkg_clear_area(1u, 1u, 18u, 14u);
    jp_bkg_clear_area(1u, 16u, 18u, 1u);
}

static void inventory_draw_menu_shell(void) BANKED {
    inventory_ui_clear();
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
}

static void inventory_put_field_text(UINT8 x, UINT8 y, UINT8 w, const char *text) BANKED {
    jp_bkg_clear_area(x, y, w, 1u);
    jp_put_bkg_text(x, y, text);
}

static void inventory_protect_footer_message(const char *message) BANKED {
    /* rpg273: The JP glyph cache is shared by the item list and the footer.
     * When the player scrolls deep into a long inventory, newly generated item
     * names can evict the footer glyphs.  Touch/redraw the footer before
     * building the next incoming row so its glyphs stay recent and the lower
     * help text does not gradually turn into garbage.
     */
    if (message == 0) message = g_inventory_default_msg;
    jp_bkg_clear_area(1u, 16u, 18u, 1u);
    jp_put_bkg_text(1u, 16u, message);
    inventory_menu_attr_rect(1u, 16u, 18u, 1u, 0u);
}

static void inventory_u8_to_dec(UINT8 value, char *out) BANKED {
    UINT8 i;

    i = 0u;
    if (value >= 10u) out[i++] = (char)('0' + (value / 10u));
    out[i++] = (char)('0' + (value % 10u));
    out[i] = '\0';
}

static void inventory_put_count(UINT8 x, UINT8 y, UINT8 value) BANKED {
    char buf[4];
    inventory_u8_to_dec(value, buf);
    jp_bkg_clear_area(x, y, 2u, 1u);
    jp_put_bkg_text(x, y, buf);
}


#define INVENTORY_REPEAT_DELAY 8u
#define INVENTORY_REPEAT_RATE   2u

static UINT8 inventory_wait_menu_keys(UINT8 mask) BANKED {
    static UINT8 last_keys;
    static UINT8 repeat_count;
    UINT8 keys;
    UINT8 pressed;
    UINT8 dirs;

    while (1) {
        keys = (UINT8)(joypad() & mask);
        pressed = (UINT8)(keys & (UINT8)~last_keys);
        if (pressed & (UINT8)(J_A | J_B | J_START | J_SELECT)) {
            last_keys = keys;
            repeat_count = 0u;
            return (UINT8)(pressed & (UINT8)(J_A | J_B | J_START | J_SELECT));
        }
        dirs = (UINT8)(keys & (UINT8)(J_UP | J_DOWN | J_LEFT | J_RIGHT));
        if (dirs != 0u) {
            if (dirs != (UINT8)(last_keys & (UINT8)(J_UP | J_DOWN | J_LEFT | J_RIGHT))) {
                last_keys = keys;
                repeat_count = 0u;
                return dirs;
            }
            if (repeat_count < INVENTORY_REPEAT_DELAY) {
                repeat_count++;
            } else {
                repeat_count = (UINT8)(INVENTORY_REPEAT_DELAY - INVENTORY_REPEAT_RATE);
                last_keys = keys;
                return dirs;
            }
        } else {
            repeat_count = 0u;
        }
        last_keys = keys;
        audio_wait_vbl();
    }
}

static void inventory_draw_page_arrows(UINT8 page_top, UINT8 visible_count) BANKED {
    if (page_top >= 2u) jp_put_bkg_text(18u, 2u, "▲");
    else jp_put_bkg_text(18u, 2u, " ");
    if ((UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT) < visible_count) jp_put_bkg_text(18u, 14u, "▼");
    else jp_put_bkg_text(18u, 14u, " ");
}

static void inventory_draw_mark(UINT8 visible_index, UINT8 page_top, UINT8 visible) BANKED {
    UINT8 offset;
    UINT8 x;
    UINT8 y;
    if (visible_index == INVENTORY_MARK_NONE) return;
    if (visible_index < page_top) return;
    offset = (UINT8)(visible_index - page_top);
    if (offset >= INVENTORY_GRID_PAGE_COUNT) return;
    x = (offset & 1u) ? 10u : 1u;
    y = (UINT8)(3u + (offset >> 1));
    jp_put_bkg_text(x, y, visible ? ">" : " ");
}

static void inventory_swap_visible_entries(UINT8 a, UINT8 b) BANKED {
    UINT8 t;
    if (a >= g_inventory_visible_count_cached || b >= g_inventory_visible_count_cached) return;
    if (a == b) return;
    t = g_inventory_visible_ids[a];
    g_inventory_visible_ids[a] = g_inventory_visible_ids[b];
    g_inventory_visible_ids[b] = t;
    g_inventory_sort_mode = INVENTORY_SORT_MANUAL;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
    inventory_invalidate_screen_cache();
}

static void inventory_rebuild_visible_cache(void) BANKED {
    UINT8 id;
    UINT8 count;

    count = 0u;
    for (id = 1u; id < INVENTORY_ITEM_MAX; id++) {
        if (g_inventory_counts[id] == 0u) continue;
        g_inventory_visible_ids[count] = id;
        count++;
    }
    g_inventory_visible_count_cached = count;
    g_inventory_visible_cache_valid = 1u;
    inventory_apply_sort_to_visible_cache();
}

static UINT8 inventory_item_kind_rank(UINT8 item_id) BANKED {
    if (inventory_is_field_usable(item_id)) return 0u;
    if ((item_id >= ITEM_WOOD_SWORD && item_id <= ITEM_TOOL_KIT) ||
        (item_id >= ITEM_COPPER_SWORD && item_id <= ITEM_GEAR_TOOL) ||
        (item_id >= ITEM_BRONZE_SPEAR && item_id <= ITEM_GREAT_AXE)) return 1u;
    if ((item_id >= ITEM_CLOTH_ARMOR && item_id <= ITEM_IRON_ARMOR) ||
        (item_id >= ITEM_TRAVEL_CLOTH && item_id <= ITEM_DRAGON_ARMOR)) return 2u;
    if ((item_id >= ITEM_CHARM && item_id <= ITEM_FEATHER) ||
        (item_id >= ITEM_QUICK_BOOTS && item_id <= ITEM_DARK_MEDAL)) return 3u;
    if (item_id >= ITEM_ITIL_SWORD && item_id <= ITEM_ITIL_CREST) return 4u;
    return 5u;
}

static UINT8 inventory_item_kana_rank(UINT8 item_id) BANKED {
    /* Compact hand-authored kana-ish rank for menu sorting.  It is intentionally
     * not a full dictionary collation table; it keeps runtime small and stable. */
    switch (item_id) {
        case ITEM_ANTIDOTE: return 1u;
        case ITEM_ICE_BOMB: return 2u;
        case ITEM_ICE_SWORD: return 3u;
        case ITEM_ELIXIR: return 4u;
        case ITEM_EYE_DROP: return 5u;
        case ITEM_HIGH_POTION: return 6u;
        case ITEM_HERB: return 7u;
        case ITEM_FULL_HERB: return 8u;
        case ITEM_WAKE_HERB: return 9u;
        case ITEM_MANA_HERB: return 10u;
        case ITEM_MANA_DROP: return 11u;
        case ITEM_MANA_BOTTLE: return 12u;
        case ITEM_MEDICINE: return 13u;
        case ITEM_NAGAI_TEA: return 14u;
        case ITEM_POTION: return 15u;
        case ITEM_PANACEA: return 16u;
        case ITEM_RETURN_FEATHER: return 16u;
        case ITEM_REVIVE_STONE: return 17u;
        case ITEM_SMOKE_BOMB: return 18u;
        case ITEM_FIRE_BOMB: return 19u;
        case ITEM_THUNDER_BOMB: return 20u;
        case ITEM_LIGHT_BOTTLE: return 21u;
        case ITEM_DARK_BOTTLE: return 22u;
        default: return (UINT8)(40u + item_id);
    }
}

static UINT8 inventory_sort_key_primary(UINT8 item_id) BANKED {
    if (g_inventory_sort_mode == INVENTORY_SORT_KIND) return inventory_item_kind_rank(item_id);
    if (g_inventory_sort_mode == INVENTORY_SORT_KANA) return inventory_item_kana_rank(item_id);
    if (g_inventory_sort_mode == INVENTORY_SORT_FIELD) return inventory_is_field_usable(item_id) ? 0u : 1u;
    if (g_inventory_sort_mode == INVENTORY_SORT_COUNT) return (UINT8)(99u - g_inventory_counts[item_id]);
    return item_id;
}

static UINT8 inventory_should_swap_sort(UINT8 a, UINT8 b) BANKED {
    UINT8 ka;
    UINT8 kb;
    ka = inventory_sort_key_primary(a);
    kb = inventory_sort_key_primary(b);
    if (ka > kb) return 1u;
    if (ka < kb) return 0u;
    return (a > b) ? 1u : 0u;
}

static void inventory_apply_sort_to_visible_cache(void) BANKED {
    UINT8 i;
    UINT8 j;
    UINT8 t;
    if (g_inventory_sort_mode == INVENTORY_SORT_MANUAL) return;
    if (g_inventory_visible_count_cached < 2u) return;
    for (i = 0u; i < (UINT8)(g_inventory_visible_count_cached - 1u); i++) {
        for (j = (UINT8)(i + 1u); j < g_inventory_visible_count_cached; j++) {
            if (inventory_should_swap_sort(g_inventory_visible_ids[i], g_inventory_visible_ids[j])) {
                t = g_inventory_visible_ids[i];
                g_inventory_visible_ids[i] = g_inventory_visible_ids[j];
                g_inventory_visible_ids[j] = t;
            }
        }
    }
}

static const char *inventory_sort_label(UINT8 mode) BANKED {
    switch (mode) {
        case INVENTORY_SORT_KIND: return "しゅるい";
        case INVENTORY_SORT_KANA: return "あいうえお";
        case INVENTORY_SORT_FIELD: return "つかえる上";
        case INVENTORY_SORT_COUNT: return "こすう多い";
        default: return "IDじゅん";
    }
}

static void inventory_draw_sort_popup(UINT8 sort_cursor) BANKED {
    UINT8 i;
    jp_draw_bkg_frame(3u, 3u, 14u, 9u);
    jp_put_bkg_text(5u, 4u, "ならびかえ");
    for (i = 0u; i < 5u; i++) {
        jp_put_bkg_text(4u, (UINT8)(6u + i), (i == sort_cursor) ? ">" : " ");
        jp_put_bkg_text(6u, (UINT8)(6u + i), inventory_sort_label(i));
    }
    inventory_menu_attr_rect(3u, 3u, 14u, 9u, 0u);
}

static void inventory_show_sort_popup(UINT8 *cursor_index, UINT8 *page_top, const char **message) BANKED {
    UINT8 keys;
    UINT8 sort_cursor;
    sort_cursor = (g_inventory_sort_mode == INVENTORY_SORT_MANUAL) ? INVENTORY_SORT_ID : g_inventory_sort_mode;
    inventory_cursor_obj_hide();
    inventory_draw_sort_popup(sort_cursor);
    while (1) {
        keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_A | J_B | J_START);
        if (keys & J_UP) {
            if (sort_cursor == 0u) sort_cursor = 4u;
            else sort_cursor--;
            inventory_draw_sort_popup(sort_cursor);
        } else if (keys & J_DOWN) {
            sort_cursor++;
            if (sort_cursor > 4u) sort_cursor = 0u;
            inventory_draw_sort_popup(sort_cursor);
        } else if (keys & (J_B | J_START)) {
            inventory_draw_full_screen_buffered(*cursor_index, *page_top, *message);
            return;
        } else if (keys & J_A) {
            g_inventory_sort_mode = sort_cursor;
            g_inventory_mark_index = INVENTORY_MARK_NONE;
            inventory_rebuild_visible_cache();
            *cursor_index = 0u;
            *page_top = 0u;
            g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
            g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
            *message = inventory_sort_label(sort_cursor);
            inventory_draw_full_screen_buffered(*cursor_index, *page_top, *message);
            return;
        }
    }
}

void inventory_precache_field_menu(void) BANKED {
    /* Safe lightweight prefetch only: ids/icons, no Japanese BG rendering while
     * another screen is visible. */
    ui_icons_load();
    if (g_inventory_visible_cache_valid == 0u) {
        inventory_rebuild_visible_cache();
    }
}

static UINT8 inventory_visible_count(void) BANKED {
    return g_inventory_visible_count_cached;
}

static UINT8 inventory_item_at_visible_index(UINT8 index) BANKED {
    if (index >= g_inventory_visible_count_cached) return ITEM_NONE;
    return g_inventory_visible_ids[index];
}

static UINT8 inventory_clamp_page_top(UINT8 page_top, UINT8 visible_count) BANKED {
    UINT8 max_top;

    /* rpg271 FF4-style item crawl:
     * page_top is now the first visible item index, not a 22-item page.
     * Keep it on a two-item row boundary so the two-column grid scrolls
     * one row at a time.
     */
    if (visible_count <= INVENTORY_GRID_PAGE_COUNT) return 0u;

    /* rpg290: allow a final one-item row to scroll into view.
     * With 11 rows x 2 columns, page_top=2 must be valid when visible_count=23.
     */
    max_top = (UINT8)(visible_count - (INVENTORY_GRID_PAGE_COUNT - 1u));
    if (max_top & 1u) max_top--;
    if (page_top > max_top) page_top = max_top;
    if (page_top & 1u) page_top--;
    return page_top;
}

static void inventory_draw_grid_entry(UINT8 visible_index, UINT8 page_top, UINT8 row, UINT8 col) BANKED {
    UINT8 id;
    UINT8 x;
    UINT8 slot;

    id = inventory_item_at_visible_index(visible_index);
    x = (col == 0u) ? 1u : 10u;
    if (id == ITEM_NONE) {
        jp_bkg_clear_area(x, row, 9u, 1u);
        return;
    }
    jp_put_bkg_text(x, row, " ");
    ui_put_icon((UINT8)(x + 1u), row, ui_icon_tile_for_item(id));
    slot = (UINT8)(visible_index - page_top);
    if (slot < INVENTORY_NAME_FIXED_SLOTS) {
        inventory_fixed_name_load_slot(slot, id);
        inventory_fixed_name_put_slot(slot, (UINT8)(x + 2u), row);
    } else {
        menu_dict_put_item_short_name((UINT8)(x + 2u), row, INVENTORY_NAME_TILE_W, id);
    }
    inventory_put_count((UINT8)(x + 7u), row, g_inventory_counts[id]);
}



static void inventory_draw_grid_entry_rel(UINT8 visible_index, UINT8 rel_x, UINT8 fixed_slot) {
    UINT8 id;
    UINT8 i;
    char buf[4];

    for (i = 0u; i < 9u; i++) {
        jp_bkg_backbuffer[rel_x + i] = (UINT8)(JP_FRAME_BASE + 0u);
    }

    id = inventory_item_at_visible_index(visible_index);
    if (id == ITEM_NONE) return;

    jp_put_bkg_text(rel_x, 0u, " ");
    ui_put_icon((UINT8)(rel_x + 1u), 0u, ui_icon_tile_for_item(id));
    inventory_fixed_name_load_slot(fixed_slot, id);
    inventory_fixed_name_put_slot(fixed_slot, (UINT8)(rel_x + 2u), 0u);
    inventory_u8_to_dec(g_inventory_counts[id], buf);
    jp_put_bkg_text((UINT8)(rel_x + 7u), 0u, buf);
}

static void inventory_build_row_into(UINT8 *dst, UINT8 first_visible_index, UINT8 physical_row) {
    UINT8 i;
    UINT8 prev_active;

    if (dst == 0) return;

    for (i = 0u; i < 20u; i++) {
        g_inventory_row_scratch[i] = jp_bkg_backbuffer[i];
        jp_bkg_backbuffer[i] = (UINT8)(JP_FRAME_BASE + 0u);
    }

    prev_active = jp_bkg_buffer_active;
    jp_bkg_buffer_active = 1u;
    inventory_draw_grid_entry_rel(first_visible_index, 0u, (UINT8)(physical_row * 2u));
    inventory_draw_grid_entry_rel((UINT8)(first_visible_index + 1u), 9u, (UINT8)((physical_row * 2u) + 1u));
    jp_bkg_buffer_active = prev_active;

    for (i = 0u; i < INVENTORY_ROW_TILE_W; i++) {
        dst[i] = jp_bkg_backbuffer[i];
    }
    for (i = 0u; i < 20u; i++) {
        jp_bkg_backbuffer[i] = g_inventory_row_scratch[i];
    }
}

static void inventory_build_row_tiles(UINT8 row_cache_index, UINT8 first_visible_index) {
    if (row_cache_index >= INVENTORY_GRID_ROWS) return;
    inventory_build_row_into(g_inventory_row_tiles[row_cache_index], first_visible_index, row_cache_index);
}

static void inventory_copy_row(UINT8 *dst, const UINT8 *src) {
    UINT8 i;
    if (dst == 0 || src == 0) return;
    for (i = 0u; i < INVENTORY_ROW_TILE_W; i++) dst[i] = src[i];
}

static void inventory_build_prefetch_rows(UINT8 page_top) {
    /* rpg278: fixed VRAM slots are tied to visible physical rows.
     * Do not pre-render hidden rows into those slots, or visible names would be replaced.
     */
    (void)page_top;
}

static void inventory_build_visible_rows_cache(UINT8 page_top) {
    UINT8 row;
    UINT8 idx;

    page_top = inventory_clamp_page_top(page_top, inventory_visible_count());
    if (g_inventory_row_cache_top == page_top) return;

    g_inventory_row_ring_base = 0u;
    idx = page_top;
    for (row = 0u; row < INVENTORY_GRID_ROWS; row++) {
        inventory_build_row_tiles(row, idx);
        idx = (UINT8)(idx + 2u);
    }
    g_inventory_row_cache_top = page_top;
    inventory_build_prefetch_rows(page_top);
}

static void inventory_put_cached_row(UINT8 row_cache_index, UINT8 y) {
    UINT8 i;
    UINT16 base;

    if (row_cache_index >= INVENTORY_GRID_ROWS) return;
    if (jp_bkg_buffer_active) {
        base = ((UINT16)y * JP_BKG_BUFFER_W) + 1u;
        for (i = 0u; i < INVENTORY_ROW_TILE_W; i++) {
            jp_bkg_backbuffer[base + i] = g_inventory_row_tiles[row_cache_index][i];
        }
    } else {
        set_bkg_tiles(1u, y, INVENTORY_ROW_TILE_W, 1u, g_inventory_row_tiles[row_cache_index]);
    }
}

static void inventory_commit_list_rows_cache(void) {
    UINT8 row;
    UINT8 y;

    y = 3u;
    for (row = 0u; row < INVENTORY_GRID_ROWS; row++, y++) {
        inventory_put_cached_row(inventory_physical_row_for_display(row), y);
    }
    inventory_menu_attr_rect(1u, 3u, INVENTORY_ROW_TILE_W, INVENTORY_GRID_ROWS, 0u);
    inventory_apply_visible_name_attrs(g_inventory_row_cache_top);
    inventory_draw_mark(g_inventory_mark_index, g_inventory_row_cache_top, 1u);
}

static void inventory_scroll_list_cache_to(UINT8 new_page_top) {
    UINT8 visible_count;
    UINT8 old_top;
    UINT8 recycled;
    UINT8 incoming_index;

    visible_count = inventory_visible_count();
    new_page_top = inventory_clamp_page_top(new_page_top, visible_count);
    old_top = g_inventory_row_cache_top;

    if (old_top == INVENTORY_ROW_CACHE_NONE) {
        inventory_build_visible_rows_cache(new_page_top);
        inventory_commit_list_rows_cache();
        return;
    }
    if (new_page_top == old_top) {
        inventory_commit_list_rows_cache();
        return;
    }

    /* rpg278: with item names in fixed VRAM bank1 slots, cached rows no longer
     * become stale when jpfont evicts title/footer glyphs.  Restore the fast
     * FF4-style one-row crawl: reuse ten rows and build only the entering row.
     */
    if (new_page_top == (UINT8)(old_top + 2u)) {
        recycled = g_inventory_row_ring_base;
        /* The recycled fixed-name slot is still visible on the top row until
         * the list is committed.  Clear only the outgoing name cells first,
         * then load the incoming glyphs.  This prevents the new bottom item
         * name from flashing at the disappearing top row.
         */
        inventory_clear_name_cells_for_screen_row(3u);
        g_inventory_row_ring_base = (UINT8)(g_inventory_row_ring_base + 1u);
        if (g_inventory_row_ring_base >= INVENTORY_GRID_ROWS) g_inventory_row_ring_base = 0u;
        incoming_index = (UINT8)(new_page_top + ((INVENTORY_GRID_ROWS - 1u) * 2u));
        inventory_build_row_tiles(recycled, incoming_index);
        g_inventory_row_cache_top = new_page_top;
        inventory_commit_list_rows_cache();
        return;
    }

    if ((old_top >= 2u) && (new_page_top == (UINT8)(old_top - 2u))) {
        /* Same protection in reverse: the recycled slot is visible on the
         * bottom row before it becomes the new top row.
         */
        inventory_clear_name_cells_for_screen_row(13u);
        if (g_inventory_row_ring_base == 0u) g_inventory_row_ring_base = (UINT8)(INVENTORY_GRID_ROWS - 1u);
        else g_inventory_row_ring_base = (UINT8)(g_inventory_row_ring_base - 1u);
        recycled = g_inventory_row_ring_base;
        inventory_build_row_tiles(recycled, new_page_top);
        g_inventory_row_cache_top = new_page_top;
        inventory_commit_list_rows_cache();
        return;
    }

    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
    inventory_build_visible_rows_cache(new_page_top);
    inventory_commit_list_rows_cache();
}

static void inventory_draw_cursor_only(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED {
    inventory_cursor_obj_move(visible_index, page_top, enabled);
}

static void inventory_draw_page_counter(UINT8 cursor_index, UINT8 visible_count) BANKED {
    char page_buf[4];

    if (visible_count > INVENTORY_GRID_PAGE_COUNT) {
        inventory_u8_to_dec((UINT8)(cursor_index + 1u), page_buf);
        jp_put_bkg_text(14u, 1u, page_buf);
        jp_put_bkg_text(16u, 1u, "/");
        inventory_u8_to_dec(visible_count, page_buf);
        jp_put_bkg_text(17u, 1u, page_buf);
    } else {
        jp_bkg_clear_area(14u, 1u, 5u, 1u);
    }
}

static void inventory_draw_items_page(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED {
    UINT8 row;
    UINT8 r;
    UINT8 visible_count;

    visible_count = inventory_visible_count();
    page_top = inventory_clamp_page_top(page_top, visible_count);
    inventory_clear_list_area();

    if (visible_count == 0u) {
        jp_put_bkg_text(2u, 6u, menu_dict_message(MENU_DICT_MSG_EMPTY));
    } else {
        row = 3u;
        g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
        inventory_build_visible_rows_cache(page_top);
        for (r = 0u; r < INVENTORY_GRID_ROWS; r++, row++) {
            inventory_put_cached_row(inventory_physical_row_for_display(r), row);
        }
        inventory_apply_visible_name_attrs(page_top);
    }

    /* Draw stable labels LAST.  Item-row generation may touch many Japanese
     * glyphs; drawing title/counter/footer after the rows keeps their glyphs
     * fresh and prevents the visible header/help text from turning into stale
     * dynamic-font tiles while scrolling.
     */
    jp_bkg_clear_area(1u, 1u, 8u, 1u);
    jp_put_bkg_text(1u, 1u, menu_dict_message(MENU_DICT_MSG_ITEMS_TITLE));
    inventory_draw_page_counter(cursor_index, visible_count);
    inventory_draw_page_arrows(page_top, visible_count);
    if (message != 0) inventory_protect_footer_message(message);
}

static void inventory_draw_full_screen_buffered(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED {
    inventory_reset_scroll_base();
    jp_reset_cache();
    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    /* rpg266 safe path:
     * Current JP glyph tiles are dynamic and shared, so off-screen tilemap
     * caches can corrupt the visible screen.  Build the selected page into the
     * backbuffer and commit it immediately; cursor/quantity updates remain
     * differential after that.
     */
    jp_bkg_buffer_begin();
    inventory_draw_menu_shell();
    inventory_draw_items_page(cursor_index, page_top, message);
    jp_bkg_buffer_commit();
    inventory_menu_attr_rect(0u, 0u, 20u, 18u, 0u);
    inventory_apply_visible_name_attrs(page_top);
    inventory_cursor_obj_prepare();
    inventory_cursor_obj_move(cursor_index, page_top, (UINT8)(inventory_visible_count() != 0u));
    inventory_draw_mark(g_inventory_mark_index, page_top, 1u);
    inventory_precache_next_page(page_top, message);
}

static void inventory_draw_full_screen_direct(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED {
    inventory_reset_scroll_base();
    jp_reset_cache();
    g_inventory_row_cache_top = INVENTORY_ROW_CACHE_NONE;
    g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
    /* rpg269: After a slide transition, avoid DISPLAY_OFF and restore the
     * canonical 0-based page directly.  It may show a tiny tile update shimmer,
     * but it removes the white flash between pages.
     */
    inventory_draw_menu_shell();
    inventory_draw_items_page(cursor_index, page_top, message);
    inventory_menu_attr_rect(0u, 0u, 20u, 18u, 0u);
    inventory_apply_visible_name_attrs(page_top);
    inventory_cursor_obj_prepare();
    inventory_cursor_obj_move(cursor_index, page_top, (UINT8)(inventory_visible_count() != 0u));
    inventory_draw_mark(g_inventory_mark_index, page_top, 1u);
    inventory_precache_next_page(page_top, message);
}


static void inventory_redraw_list_for_row_scroll(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED {
    UINT8 visible_count;

    /* rpg277: redraw only the item-list rectangle, then redraw title/counter/
     * footer last.  This is intentionally less aggressive than the previous
     * one-row-only cache, because stale dynamic-font tile IDs were corrupting
     * the first/last item row and the help text during rapid scrolling.
     */
    visible_count = inventory_visible_count();
    inventory_scroll_list_cache_to(page_top);
    jp_bkg_clear_area(1u, 1u, 8u, 1u);
    jp_put_bkg_text(1u, 1u, menu_dict_message(MENU_DICT_MSG_ITEMS_TITLE));
    inventory_draw_page_counter(cursor_index, visible_count);
    inventory_draw_page_arrows(page_top, visible_count);
    inventory_protect_footer_message(message);
    inventory_cursor_obj_prepare();
    inventory_cursor_obj_move(cursor_index, page_top, (UINT8)(visible_count != 0u));
}



static UINT8 inventory_wrap_bgy(UINT8 y) {
    return (UINT8)(y & 31u);
}

static UINT8 inventory_frame_tile_for_rect(UINT8 x, UINT8 y, UINT8 w, UINT8 h) {
    if (w < 2u || h < 2u) return (UINT8)(JP_FRAME_BASE + 0u);
    if (y == 0u && x == 0u) return (UINT8)(JP_FRAME_BASE + 3u);
    if (y == 0u && x == (UINT8)(w - 1u)) return (UINT8)(JP_FRAME_BASE + 4u);
    if (y == (UINT8)(h - 1u) && x == (UINT8)(w - 1u)) return (UINT8)(JP_FRAME_BASE + 5u);
    if (y == (UINT8)(h - 1u) && x == 0u) return (UINT8)(JP_FRAME_BASE + 6u);
    if (y == 0u) return (UINT8)(JP_FRAME_BASE + 1u);
    if (x == (UINT8)(w - 1u)) return (UINT8)(JP_FRAME_BASE + 2u);
    if (y == (UINT8)(h - 1u)) return (UINT8)(JP_FRAME_BASE + 7u);
    if (x == 0u) return (UINT8)(JP_FRAME_BASE + 8u);
    return (UINT8)(JP_FRAME_BASE + 0u);
}

static void inventory_draw_shell_row_at(UINT8 logical_y, UINT8 map_y) {
    UINT8 x;
    UINT8 t;
    UINT8 row[20u];

    for (x = 0u; x < 20u; x++) {
        t = inventory_frame_tile_for_rect(x, logical_y, 20u, 18u);
        if (logical_y >= 15u) {
            t = inventory_frame_tile_for_rect(x, (UINT8)(logical_y - 15u), 20u, 3u);
        }
        row[x] = t;
    }
    set_bkg_tiles(0u, map_y, 20u, 1u, row);
    inventory_menu_attr_rect(0u, map_y, 20u, 1u, 0u);
}

static void inventory_draw_page_counter_at(UINT8 cursor_index, UINT8 visible_count, UINT8 map_y) {
    char page_buf[4];

    if (visible_count > INVENTORY_GRID_PAGE_COUNT) {
        inventory_u8_to_dec((UINT8)(cursor_index + 1u), page_buf);
        jp_put_bkg_text(14u, map_y, page_buf);
        jp_put_bkg_text(16u, map_y, "/");
        inventory_u8_to_dec(visible_count, page_buf);
        jp_put_bkg_text(17u, map_y, page_buf);
    } else {
        jp_bkg_clear_area(14u, map_y, 5u, 1u);
    }
}

static void inventory_draw_page_arrows_at(UINT8 page_top, UINT8 visible_count, UINT8 logical_y, UINT8 map_y) {
    if (logical_y == 2u) {
        if (page_top >= 2u) jp_put_bkg_text(18u, map_y, "▲");
        else jp_put_bkg_text(18u, map_y, " ");
    } else if (logical_y == 14u) {
        if ((UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT) < visible_count) jp_put_bkg_text(18u, map_y, "▼");
        else jp_put_bkg_text(18u, map_y, " ");
    }
}

static void inventory_draw_grid_entry_at(UINT8 visible_index, UINT8 page_top, UINT8 logical_row, UINT8 map_y, UINT8 col) {
    (void)logical_row;
    inventory_draw_grid_entry(visible_index, page_top, map_y, col);
}

static void inventory_draw_page_row_at(UINT8 cursor_index, UINT8 page_top, const char *message, UINT8 logical_y, UINT8 map_y) {
    UINT8 visible_count;
    UINT8 r;
    UINT8 idx;

    visible_count = inventory_visible_count();
    page_top = inventory_clamp_page_top(page_top, visible_count);
    inventory_draw_shell_row_at(logical_y, map_y);

    if (logical_y == 1u) {
        jp_put_bkg_text(1u, map_y, menu_dict_message(MENU_DICT_MSG_ITEMS_TITLE));
        inventory_draw_page_counter_at(cursor_index, visible_count, map_y);
        return;
    }

    if (visible_count == 0u) {
        if (logical_y == 6u) jp_put_bkg_text(2u, map_y, menu_dict_message(MENU_DICT_MSG_EMPTY));
        if (logical_y == 16u && message != 0) inventory_put_field_text(1u, map_y, 18u, message);
        return;
    }

    if (logical_y >= 3u && logical_y < (UINT8)(3u + INVENTORY_GRID_ROWS)) {
        r = (UINT8)(logical_y - 3u);
        idx = (UINT8)(page_top + (UINT8)(r * 2u));
        inventory_draw_grid_entry_at(idx, page_top, logical_y, map_y, 0u);
        idx++;
        inventory_draw_grid_entry_at(idx, page_top, logical_y, map_y, 1u);
        return;
    }

    inventory_draw_page_arrows_at(page_top, visible_count, logical_y, map_y);
    if (logical_y == 16u && message != 0) inventory_put_field_text(1u, map_y, 18u, message);
}

static void inventory_reset_scroll_base(void) {
    g_inventory_bgy_base = 0u;
    move_bkg(0u, 0u);
}

static void inventory_normalize_after_slide(UINT8 cursor_index, UINT8 page_top, const char *message) {
    if (g_inventory_bgy_base == 0u) return;
    /* Normalize only when an action needs normal logical rows.  Do not redraw
     * at the end of the slide itself, because that was the perceived reload. */
    inventory_draw_full_screen_direct(cursor_index, page_top, message);
}

static void inventory_slide_page_transition(UINT8 cursor_index, UINT8 old_page_top, UINT8 new_page_top, const char *message) {
    UINT8 frame;
    UINT8 logical_y;
    UINT8 map_y;
    UINT8 scroll_px;
    UINT8 reverse;
    UINT8 old_base;
    UINT8 new_base;

    inventory_cursor_obj_hide();
    reverse = (new_page_top < old_page_top) ? 1u : 0u;
    old_base = g_inventory_bgy_base;
    new_base = reverse ? inventory_wrap_bgy((UINT8)(old_base + 14u)) : inventory_wrap_bgy((UINT8)(old_base + 18u));
    scroll_px = (UINT8)(old_base << 3);
    move_bkg(0u, scroll_px);

    if (reverse == 0u) {
        /* Next page: current page goes upward, next page rises from below.
         * Draw the incoming page directly into its final ring-buffer rows. */
        for (frame = 0u; frame < 18u; frame++) {
            logical_y = frame;
            map_y = inventory_wrap_bgy((UINT8)(new_base + logical_y));
            inventory_draw_page_row_at(cursor_index, new_page_top, message, logical_y, map_y);

            scroll_px = (UINT8)(scroll_px + 8u);
            audio_wait_vbl();
            move_bkg(0u, scroll_px);
        }
    } else {
        /* Previous page: reverse scroll.  The previous page comes down from above. */
        for (frame = 0u; frame < 18u; frame++) {
            logical_y = (UINT8)(17u - frame);
            map_y = inventory_wrap_bgy((UINT8)(new_base + logical_y));
            inventory_draw_page_row_at(cursor_index, new_page_top, message, logical_y, map_y);

            scroll_px = (UINT8)(scroll_px - 8u);
            audio_wait_vbl();
            move_bkg(0u, scroll_px);
        }
    }

    g_inventory_bgy_base = new_base;
    inventory_cursor_obj_prepare();
    inventory_cursor_obj_move(cursor_index, new_page_top, (UINT8)(inventory_visible_count() != 0u));
}


static UINT8 inventory_cursor_down(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED {
    UINT8 next;

    if (visible_count == 0u) return 0u;
    if ((UINT8)(cursor_index + 2u) >= visible_count) return cursor_index;

    next = (UINT8)(cursor_index + 2u);
    if (next >= (UINT8)(*page_top + INVENTORY_GRID_PAGE_COUNT)) {
        *page_top = inventory_clamp_page_top((UINT8)(*page_top + 2u), visible_count);
    }
    return next;
}

static UINT8 inventory_cursor_up(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED {
    UINT8 next;

    if (visible_count == 0u) return 0u;
    if (cursor_index < 2u) return cursor_index;

    next = (UINT8)(cursor_index - 2u);
    if (next < *page_top) {
        if (*page_top >= 2u) *page_top = inventory_clamp_page_top((UINT8)(*page_top - 2u), visible_count);
        else *page_top = 0u;
    }
    return next;
}

static UINT8 inventory_is_field_usable(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HERB:
        case ITEM_POTION:
        case ITEM_MANA_HERB:
        case ITEM_ANTIDOTE:
        case ITEM_MEDICINE:
        case ITEM_HIGH_POTION:
        case ITEM_NAGAI_TEA:
        case ITEM_ELIXIR:
        case ITEM_MANA_DROP:
        case ITEM_MANA_BOTTLE:
        case ITEM_FULL_HERB:
        case ITEM_PANACEA:
        case ITEM_WAKE_HERB:
        case ITEM_EYE_DROP:
        case ITEM_BARRIER_SEED:
        case ITEM_POWER_SEED:
        case ITEM_GUARD_SEED:
        case ITEM_SPEED_SEED:
        case ITEM_FOCUS_TEA:
        case ITEM_MORALE_MEAT:
        case ITEM_TENT:
        case ITEM_REVIVE_STONE:
            return 1u;
        default:
            return 0u;
    }
}

static UINT8 inventory_effect_is_growth(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_BARRIER_SEED:
        case ITEM_GUARD_SEED:
        case ITEM_POWER_SEED:
        case ITEM_SPEED_SEED:
        case ITEM_FOCUS_TEA:
        case ITEM_MORALE_MEAT:
            return 1u;
        default:
            return 0u;
    }
}

static const char *inventory_make_item_result_message(UINT8 item_id) BANKED {
    UINT8 amount;
    UINT8 kind;
    char num[4];

    amount = party_get_last_item_effect_amount();
    kind = party_get_last_item_effect_kind();

    if (amount != 0u) {
        inventory_u8_to_dec(amount, num);
        inventory_item_effect_message[0] = num[0];
        inventory_item_effect_message[1] = num[1] ? num[1] : ' ';
        inventory_item_effect_message[2] = num[1] ? (num[2] ? num[2] : ' ') : ' ';
        inventory_item_effect_message[3] = '\0';
        if (inventory_effect_is_growth(item_id)) {
            inventory_item_effect_message[1] = '\0';
            if (num[1]) {
                inventory_item_effect_message[0] = num[0];
                inventory_item_effect_message[1] = num[1];
                inventory_item_effect_message[2] = num[2];
                inventory_item_effect_message[3] = '\0';
            }
            inventory_put_field_text(1u, 16u, 18u, inventory_item_effect_message);
            return menu_dict_message(MENU_DICT_MSG_UPPED);
        }
        (void)kind;
        inventory_put_field_text(1u, 16u, 18u, inventory_item_effect_message);
        return menu_dict_message(MENU_DICT_MSG_HEAL);
    }
    return menu_dict_message(MENU_DICT_MSG_USED);
}

static void inventory_draw_target_popup(UINT8 item_id, UINT8 slot_cursor) BANKED {
    UINT8 i;

    jp_draw_bkg_frame(10u, 2u, 10u, 9u);
    ui_put_icon(11u, 3u, ui_icon_tile_for_item(item_id));
    inventory_put_field_text(12u, 3u, 7u, inventory_item_name(item_id));
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        jp_put_bkg_text(11u, (UINT8)(5u + i), (i == slot_cursor) ? ">" : " ");
        inventory_put_field_text(12u, (UINT8)(5u + i), 7u, party_get_active_name(i));
    }
    inventory_put_field_text(11u, 9u, 8u, "A=使う");
    inventory_menu_attr_rect(10u, 2u, 10u, 9u, 0u);
}

void inventory_menu_show_items_loop(void) BANKED {
    UINT8 keys;
    UINT8 cursor_index;
    UINT8 page_top;
    UINT8 target_slot;
    UINT8 result;
    UINT8 cursor_item;
    UINT8 visible_count;
    const char *msg;
    UINT8 popup_redraw;
    UINT8 old_cursor;
    UINT8 old_page;
    UINT8 redraw_full;

    inventory_precache_field_menu();
    inventory_hide_all_oam_slots();
    HIDE_SPRITES;
    visible_count = inventory_visible_count();
    cursor_index = 0u;
    page_top = 0u;
    target_slot = 0u;
    msg = g_inventory_default_msg;
    audio_waitpadup();
    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);
    while (1) {
        visible_count = inventory_visible_count();
        if (visible_count == 0u) cursor_index = 0u;
        else if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
        page_top = inventory_clamp_page_top(page_top, visible_count);

        keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START | J_SELECT);
        redraw_full = 0u;
        old_cursor = cursor_index;
        old_page = page_top;
        if (keys & J_UP) {
            cursor_index = inventory_cursor_up(cursor_index, &page_top, visible_count);
        } else if (keys & J_DOWN) {
            cursor_index = inventory_cursor_down(cursor_index, &page_top, visible_count);
        } else if (keys & J_LEFT) {
            if ((visible_count != 0u) && (cursor_index & 1u) && (cursor_index > page_top)) cursor_index--;
        } else if (keys & J_RIGHT) {
            if ((visible_count != 0u) && ((cursor_index & 1u) == 0u) && ((UINT8)(cursor_index + 1u) < visible_count) && ((UINT8)(cursor_index + 1u) < (UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT))) cursor_index++;
        } else if (keys & J_B) {
            inventory_reset_scroll_base();
            break;
        } else if (keys & J_START) {
            inventory_show_sort_popup(&cursor_index, &page_top, &msg);
            continue;
        } else if (keys & J_SELECT) {
            if (visible_count == 0u) continue;
            if (g_inventory_mark_index == INVENTORY_MARK_NONE) {
                g_inventory_mark_index = cursor_index;
                inventory_draw_mark(g_inventory_mark_index, page_top, 1u);
                msg = "SELECTでいれかえ";
                inventory_protect_footer_message(msg);
            } else if (g_inventory_mark_index == cursor_index) {
                inventory_draw_mark(g_inventory_mark_index, page_top, 0u);
                g_inventory_mark_index = INVENTORY_MARK_NONE;
                msg = g_inventory_default_msg;
                inventory_protect_footer_message(msg);
            } else {
                inventory_swap_visible_entries(g_inventory_mark_index, cursor_index);
                g_inventory_mark_index = INVENTORY_MARK_NONE;
                msg = "いれかえました";
                inventory_redraw_list_for_row_scroll(cursor_index, page_top, msg);
            }
            inventory_cursor_obj_move(cursor_index, page_top, 1u);
            continue;
        } else if (keys & J_A) {
            if (visible_count == 0u) continue;
            cursor_item = inventory_item_at_visible_index(cursor_index);
            if (cursor_item == ITEM_NONE) continue;
            if (!inventory_is_field_usable(cursor_item)) {
                msg = menu_dict_message(MENU_DICT_MSG_CANNOT_USE);
                inventory_protect_footer_message(msg);
                inventory_cursor_obj_move(cursor_index, page_top, 1u);
                continue;
            }
            inventory_draw_items_page(cursor_index, page_top, menu_dict_message(MENU_DICT_MSG_TARGET));
            inventory_draw_target_popup(cursor_item, target_slot);
            while (1) {
                keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_A | J_B);
                popup_redraw = 0u;
                if (keys & J_UP) {
                    if (target_slot == 0u) target_slot = PARTY_ACTIVE_COUNT - 1u;
                    else target_slot--;
                    popup_redraw = 1u;
                } else if (keys & J_DOWN) {
                    target_slot++;
                    if (target_slot >= PARTY_ACTIVE_COUNT) target_slot = 0u;
                    popup_redraw = 1u;
                } else if (keys & J_B) {
                    msg = g_inventory_default_msg;
                    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);
                    break;
                } else if (keys & J_A) {
                    result = party_use_field_item_on_active(cursor_item, target_slot);
                    if (result != 0u) {
                        inventory_remove(cursor_item, 1u);
                        msg = inventory_make_item_result_message(cursor_item);
                        if (g_inventory_counts[cursor_item] != 0u) {
                            inventory_update_quantity_only(cursor_index, page_top);
                            inventory_protect_footer_message(msg);
                            inventory_cursor_obj_move(cursor_index, page_top, 1u);
                            break;
                        }
                        inventory_rebuild_visible_cache();
                        visible_count = inventory_visible_count();
                        if (visible_count == 0u) cursor_index = 0u;
                        else if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
                        page_top = inventory_clamp_page_top(page_top, visible_count);
                        g_inventory_page_cached_top = INVENTORY_PAGE_TOP_NONE;
                    } else {
                        msg = menu_dict_message(MENU_DICT_MSG_NO_EFFECT);
                    }
                    inventory_draw_full_screen_buffered(cursor_index, page_top, msg);
                    break;
                }
                if (popup_redraw != 0u) {
                    inventory_draw_target_popup(cursor_item, target_slot);
                }
            }
        }

        if ((keys & (J_UP | J_DOWN | J_LEFT | J_RIGHT)) && (old_cursor != cursor_index || old_page != page_top)) {
            if (old_page != page_top) {
                redraw_full = 1u;
            }
            if (redraw_full != 0u) {
                inventory_redraw_list_for_row_scroll(cursor_index, page_top, msg);
            } else {
                /* rpg270: cursor movement after slide is OBJ-only.
                 * Keep the completed BG map in place and avoid hidden redraws. */
                inventory_draw_cursor_only(old_cursor, page_top, 0u);
                inventory_draw_cursor_only(cursor_index, page_top, 1u);
            }
        }
    }
    inventory_reset_scroll_base();
    inventory_cursor_obj_hide();
    HIDE_SPRITES;
}

