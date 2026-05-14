#pragma bank 24

#include <gb/gb.h>
#include <gb/hardware.h>
#include "equip_runtime.h"
#include "party_runtime.h"
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"
#include "ui_icons.h"

BANKREF(equip_runtime_bank)

#ifndef VBK_TILES
#define VBK_TILES 0u
#endif
#ifndef VBK_ATTRIBUTES
#define VBK_ATTRIBUTES 1u
#endif

static UINT8 equip_attr_row[20u];

static void equip_attr_rect0(UINT8 x, UINT8 y, UINT8 w, UINT8 h) {
    UINT8 i;
    UINT8 yy;
    if (w == 0u || h == 0u) return;
    if (w > 20u) w = 20u;
    for (i = 0u; i < w; i++) equip_attr_row[i] = 0u;
    VBK_REG = VBK_ATTRIBUTES;
    for (yy = 0u; yy < h; yy++) {
        set_bkg_tiles(x, (UINT8)(y + yy), w, 1u, equip_attr_row);
    }
    VBK_REG = VBK_TILES;
}

#define EQUIP_CANDIDATE_MAX 48u
#define EQUIP_POPUP_ROWS 5u

static UINT8 equip_choices[EQUIP_CANDIDATE_MAX];
static UINT8 equip_precache_valid;
static UINT8 equip_precache_active_slot;
static UINT8 equip_precache_slot_cursor;
static UINT8 equip_precache_item_count;

static void equip_draw_popup(UINT8 active_slot, UINT8 slot_cursor, UINT8 item_cursor, UINT8 item_count) BANKED;

static void equip_put_bottom_message(const char *message) BANKED {
    jp_bkg_clear_area(1u, 16u, 18u, 1u);
    jp_put_bkg_text(1u, 16u, message);
}

static const char *equip_slot_label(UINT8 slot) BANKED {
    if (slot == PARTY_EQUIP_SLOT_WEAPON) return "ぶき";
    if (slot == PARTY_EQUIP_SLOT_ARMOR) return "よろい";
    return "かざり";
}

static void equip_put_u8(UINT8 x, UINT8 y, UINT8 value) BANKED {
    char buf[4];

    buf[0] = (char)('0' + (value / 100u));
    buf[1] = (char)('0' + ((value / 10u) % 10u));
    buf[2] = (char)('0' + (value % 10u));
    buf[3] = '\0';
    if (buf[0] == '0') {
        buf[0] = ' ';
        if (buf[1] == '0') buf[1] = ' ';
    }
    jp_put_bkg_text(x, y, buf);
}

static void equip_put_compare_u8(UINT8 x, UINT8 y, UINT8 old_value, UINT8 new_value) BANKED {
    equip_put_u8(x, y, old_value);
    jp_put_bkg_text((UINT8)(x + 3u), y, ">");
    equip_put_u8((UINT8)(x + 5u), y, new_value);
}

static void equip_put_item_icon(UINT8 x, UINT8 y, UINT8 item_id) BANKED {
    ui_put_icon(x, y, ui_icon_tile_for_item(item_id));
}

static void equip_draw_base_page(UINT8 active_slot, UINT8 slot_cursor, const char *message) BANKED {
    UINT8 item_id;

    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
    jp_put_bkg_text(1u, 1u, "そうび");
    jp_put_bkg_text(13u, 1u, "なかま");
    party_equip_put_active_name(1u, 3u, 12u, active_slot);

    jp_put_bkg_text(1u, 5u, (slot_cursor == PARTY_EQUIP_SLOT_WEAPON) ? ">" : " ");
    jp_put_bkg_text(3u, 5u, "ぶき");
    item_id = party_equip_get_slot_item(active_slot, PARTY_EQUIP_SLOT_WEAPON);
    equip_put_item_icon(7u, 5u, item_id);
    party_equip_put_item_name(8u, 5u, 10u, item_id);

    jp_put_bkg_text(1u, 6u, (slot_cursor == PARTY_EQUIP_SLOT_ARMOR) ? ">" : " ");
    jp_put_bkg_text(3u, 6u, "よろい");
    item_id = party_equip_get_slot_item(active_slot, PARTY_EQUIP_SLOT_ARMOR);
    equip_put_item_icon(7u, 6u, item_id);
    party_equip_put_item_name(8u, 6u, 10u, item_id);

    jp_put_bkg_text(1u, 7u, (slot_cursor == PARTY_EQUIP_SLOT_ACC) ? ">" : " ");
    jp_put_bkg_text(3u, 7u, "かざり");
    item_id = party_equip_get_slot_item(active_slot, PARTY_EQUIP_SLOT_ACC);
    equip_put_item_icon(7u, 7u, item_id);
    party_equip_put_item_name(8u, 7u, 10u, item_id);

    jp_put_bkg_text(1u, 9u, "しゅるい");
    ui_put_icon(9u, 9u, ui_icon_tile_for_weapon_type(party_equip_get_weapon_type(active_slot)));
    party_equip_put_weapon_type_name(10u, 9u, 8u, active_slot);

    jp_put_bkg_text(1u, 10u, "こうげき");
    equip_put_u8(11u, 10u, party_equip_get_attack_value(active_slot));
    jp_put_bkg_text(1u, 11u, "まもり");
    equip_put_u8(11u, 11u, party_equip_get_defense_value(active_slot));
    jp_put_bkg_text(1u, 12u, "おもさ");
    equip_put_u8(11u, 12u, party_equip_get_weight_value(active_slot));

    equip_put_bottom_message(message);
}

static void equip_draw_candidate_detail(UINT8 active_slot, UINT8 slot_cursor, UINT8 item_id) BANKED {
    UINT8 weapon_type;

    jp_bkg_clear_area(1u, 11u, 17u, 4u);

    if (slot_cursor == PARTY_EQUIP_SLOT_WEAPON) {
        weapon_type = party_equip_get_item_weapon_type(item_id);
        jp_put_bkg_text(1u, 11u, "しゅるい");
        ui_put_icon(9u, 11u, ui_icon_tile_for_weapon_type(weapon_type));
        party_equip_put_weapon_type_value_name(10u, 11u, 8u, weapon_type);
    }

    jp_put_bkg_text(1u, 12u, "こうげき");
    equip_put_compare_u8(10u, 12u,
        party_equip_get_current_slot_attack_value(active_slot, slot_cursor),
        party_equip_get_item_attack_value(item_id));

    jp_put_bkg_text(1u, 13u, "まもり");
    equip_put_compare_u8(10u, 13u,
        party_equip_get_current_slot_defense_value(active_slot, slot_cursor),
        party_equip_get_item_defense_value(item_id));

    jp_put_bkg_text(1u, 14u, "おもさ");
    equip_put_compare_u8(10u, 14u,
        party_equip_get_current_slot_weight_value(active_slot, slot_cursor),
        party_equip_get_item_weight_value(item_id));
}

static UINT8 equip_popup_page_top(UINT8 item_cursor) BANKED {
    return (UINT8)((item_cursor / EQUIP_POPUP_ROWS) * EQUIP_POPUP_ROWS);
}

static void equip_update_popup_cursor(UINT8 old_cursor, UINT8 item_cursor, UINT8 active_slot, UINT8 slot_cursor, UINT8 item_count) BANKED {
    UINT8 old_page;
    UINT8 new_page;
    UINT8 old_y;
    UINT8 new_y;
    if (item_count == 0u) return;
    old_page = equip_popup_page_top(old_cursor);
    new_page = equip_popup_page_top(item_cursor);
    if (old_page != new_page) {
        equip_draw_popup(active_slot, slot_cursor, item_cursor, item_count);
        return;
    }
    old_y = (UINT8)(5u + (UINT8)(old_cursor - old_page));
    new_y = (UINT8)(5u + (UINT8)(item_cursor - new_page));
    jp_put_bkg_text(10u, old_y, " ");
    jp_put_bkg_text(10u, new_y, ">");
    equip_draw_candidate_detail(active_slot, slot_cursor, equip_choices[item_cursor]);
    equip_attr_rect0(10u, old_y, 1u, 1u);
    equip_attr_rect0(10u, new_y, 1u, 1u);
}

static void equip_draw_popup(UINT8 active_slot, UINT8 slot_cursor, UINT8 item_cursor, UINT8 item_count) BANKED {
    UINT8 i;
    UINT8 idx;
    UINT8 page_top;
    UINT8 y;

    jp_bkg_clear_area(9u, 3u, 11u, 9u);
    jp_draw_bkg_frame(9u, 3u, 11u, 9u);
    jp_put_bkg_text(10u, 4u, equip_slot_label(slot_cursor));

    page_top = equip_popup_page_top(item_cursor);
    if (page_top > 0u) jp_put_bkg_text(18u, 4u, "う");
    if ((UINT8)(page_top + EQUIP_POPUP_ROWS) < item_count) jp_put_bkg_text(18u, 11u, "し");

    y = 5u;
    for (i = 0u; i < EQUIP_POPUP_ROWS; i++, y++) {
        idx = (UINT8)(page_top + i);
        if (idx < item_count) {
            jp_put_bkg_text(10u, y, (idx == item_cursor) ? ">" : " ");
            equip_put_item_icon(11u, y, equip_choices[idx]);
            party_equip_put_item_name(12u, y, 6u, equip_choices[idx]);
        }
    }

    if (item_count != 0u) {
        equip_draw_candidate_detail(active_slot, slot_cursor, equip_choices[item_cursor]);
    }
    equip_attr_rect0(9u, 3u, 11u, 12u);
}

static void equip_draw_full(UINT8 active_slot, UINT8 slot_cursor, UINT8 choosing, UINT8 item_cursor, UINT8 item_count, const char *message) BANKED {
    equip_draw_base_page(active_slot, slot_cursor, message);
    if (choosing != 0u) {
        equip_draw_popup(active_slot, slot_cursor, item_cursor, item_count);
    }
}

static void equip_draw_full_buffered(UINT8 active_slot, UINT8 slot_cursor, UINT8 choosing, UINT8 item_cursor, UINT8 item_count, const char *message) BANKED {
    /* rpg268: equipment screen used the generic full-screen backbuffer commit,
     * which blanks the display and could wedge on some emulator/mobile paths.
     * Draw directly and immediately reset the CGB attributes to palette 0.
     */
    equip_draw_full(active_slot, slot_cursor, choosing, item_cursor, item_count, message);
    equip_attr_rect0(0u, 0u, 20u, 18u);
}

#define EQUIP_REPEAT_DELAY 12u
#define EQUIP_REPEAT_RATE   4u

static UINT8 equip_wait_keys_raw(void) BANKED {
    static UINT8 last_keys;
    static UINT8 repeat_count;
    UINT8 keys;
    UINT8 pressed;
    UINT8 dirs;

    while (1) {
        keys = (UINT8)(joypad() & (J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START));
        pressed = (UINT8)(keys & (UINT8)~last_keys);
        if (pressed & (UINT8)(J_A | J_B | J_START)) {
            last_keys = keys;
            repeat_count = 0u;
            return (UINT8)(pressed & (UINT8)(J_A | J_B | J_START));
        }

        dirs = (UINT8)(keys & (UINT8)(J_UP | J_DOWN | J_LEFT | J_RIGHT));
        if (dirs != 0u) {
            if (dirs != (UINT8)(last_keys & (UINT8)(J_UP | J_DOWN | J_LEFT | J_RIGHT))) {
                last_keys = keys;
                repeat_count = 0u;
                return dirs;
            }
            if (repeat_count < EQUIP_REPEAT_DELAY) {
                repeat_count++;
            } else {
                repeat_count = (UINT8)(EQUIP_REPEAT_DELAY - EQUIP_REPEAT_RATE);
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

void equip_runtime_precache(void) BANKED {
    ui_icons_load();
    equip_precache_active_slot = 0u;
    equip_precache_slot_cursor = PARTY_EQUIP_SLOT_WEAPON;
    equip_precache_item_count = party_equip_build_candidates(equip_precache_active_slot, equip_precache_slot_cursor, equip_choices, EQUIP_CANDIDATE_MAX);
    equip_precache_valid = 1u;
}

void equip_runtime_show_loop(void) BANKED {
    UINT8 keys;
    UINT8 slot;
    UINT8 slot_cursor;
    UINT8 choosing;
    UINT8 item_cursor;
    UINT8 old_cursor;
    UINT8 item_count;
    UINT8 i;
    const char *message;

    slot = 0u;
    slot_cursor = PARTY_EQUIP_SLOT_WEAPON;
    choosing = 0u;
    item_cursor = 0u;
    old_cursor = 0u;
    item_count = 0u;
    message = "えらぶ もどる";

    if (equip_precache_valid == 0u) {
        for (i = 0u; i < EQUIP_CANDIDATE_MAX; i++) {
            equip_choices[i] = ITEM_NONE;
        }
    }

    HIDE_WIN;
    HIDE_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;
    ui_icons_load();
    audio_waitpadup();

    equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);

    while (1) {
        keys = equip_wait_keys_raw();

        if (choosing == 0u) {
            if (keys & J_UP) {
                if (slot_cursor == 0u) slot_cursor = PARTY_EQUIP_SLOT_ACC;
                else slot_cursor--;
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
            } else if (keys & J_DOWN) {
                slot_cursor++;
                if (slot_cursor > PARTY_EQUIP_SLOT_ACC) slot_cursor = PARTY_EQUIP_SLOT_WEAPON;
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
            } else if (keys & J_LEFT) {
                if (slot == 0u) slot = PARTY_ACTIVE_COUNT - 1u;
                else slot--;
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
            } else if (keys & J_RIGHT) {
                slot++;
                if (slot >= PARTY_ACTIVE_COUNT) slot = 0u;
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
            } else if (keys & (J_B | J_START)) {
                break;
            } else if (keys & J_A) {
                if (equip_precache_valid != 0u && equip_precache_active_slot == slot && equip_precache_slot_cursor == slot_cursor) {
                    item_count = equip_precache_item_count;
                } else {
                    item_count = party_equip_build_candidates(slot, slot_cursor, equip_choices, EQUIP_CANDIDATE_MAX);
                }
                equip_precache_valid = 0u;
                item_cursor = 0u;
                if (item_count == 0u) {
                    message = "こうほなし";
                    choosing = 0u;
                } else {
                    choosing = 1u;
                    message = "そうび やめる";
                }
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
            }
        } else {
            old_cursor = item_cursor;
            if (keys & J_UP) {
                if (item_cursor > 0u) item_cursor--;
            } else if (keys & J_DOWN) {
                if ((UINT8)(item_cursor + 1u) < item_count) item_cursor++;
            } else if (keys & J_LEFT) {
                if (item_cursor >= EQUIP_POPUP_ROWS) item_cursor = (UINT8)(item_cursor - EQUIP_POPUP_ROWS);
            } else if (keys & J_RIGHT) {
                if ((UINT8)(item_cursor + EQUIP_POPUP_ROWS) < item_count) item_cursor = (UINT8)(item_cursor + EQUIP_POPUP_ROWS);
            } else if (keys & J_B) {
                choosing = 0u;
                message = "えらぶ もどる";
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
                continue;
            } else if (keys & J_A) {
                if (item_count == 0u) {
                    message = "こうほなし";
                    choosing = 0u;
                    equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
                    continue;
                }
                if (party_equip_apply_choice(slot, slot_cursor, equip_choices[item_cursor])) {
                    message = "そうびしました";
                } else {
                    message = "できません";
                }
                equip_precache_valid = 0u;
                choosing = 0u;
                equip_draw_full_buffered(slot, slot_cursor, choosing, item_cursor, item_count, message);
                continue;
            }

            if (old_cursor != item_cursor) {
                equip_update_popup_cursor(old_cursor, item_cursor, slot, slot_cursor, item_count);
            }
        }
    }
}
