#pragma bank 7

#include <gb/gb.h>
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"
#include "party_runtime.h"


static UINT8 g_inventory_counts[INVENTORY_ITEM_MAX];
static UINT16 g_inventory_money;

void inventory_clear(void) BANKED {
    UINT8 i;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        g_inventory_counts[i] = 0u;
    }
    g_inventory_money = 0u;
}

void inventory_seed_defaults(void) BANKED {
    inventory_clear();
    inventory_add(ITEM_HERB, 6u);
    inventory_add(ITEM_MANA_HERB, 2u);
    inventory_add(ITEM_ANTIDOTE, 1u);
    inventory_add(ITEM_CASTLE_KEY, 1u);
    inventory_add(ITEM_WOOD_SWORD, 1u);
    inventory_add(ITEM_SHORT_BOW, 1u);
    inventory_add(ITEM_TOOL_KIT, 1u);
    inventory_add(ITEM_CLOTH_ARMOR, 1u);
    inventory_add(ITEM_CHARM, 1u);
    inventory_add(ITEM_DEBUG_NO_ENCOUNT, 1u);
    inventory_add(ITEM_DEBUG_ESCAPE, 1u);
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
        return 0u;
    }

    g_inventory_counts[item_id] = (UINT8)total;
    return 1u;
}

UINT8 inventory_remove(UINT8 item_id, UINT8 amount) BANKED {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;
    if (g_inventory_counts[item_id] < amount) return 0u;

    g_inventory_counts[item_id] = (UINT8)(g_inventory_counts[item_id] - amount);
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
}


static const char *inventory_item_name(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HERB: return "やくそう";
        case ITEM_POTION: return "ポーション";
        case ITEM_CASTLE_KEY: return "しろのかぎ";
        case ITEM_MANA_HERB: return "まほう草";
        case ITEM_ANTIDOTE: return "どくけし";
        case ITEM_WOOD_SWORD: return "木のけん";
        case ITEM_IRON_SWORD: return "鉄のけん";
        case ITEM_OAK_STAFF: return "木のつえ";
        case ITEM_MAGE_STAFF: return "まどうのつえ";
        case ITEM_SHORT_BOW: return "短いゆみ";
        case ITEM_GLOVES: return "けいこ手甲";
        case ITEM_TOOL_KIT: return "道具キット";
        case ITEM_CLOTH_ARMOR: return "ぬののふく";
        case ITEM_LEATHER_ARMOR: return "かわよろい";
        case ITEM_IRON_ARMOR: return "鉄よろい";
        case ITEM_CHARM: return "おまもり";
        case ITEM_FEATHER: return "はね飾り";
        case ITEM_DEBUG_NO_ENCOUNT: return "退魔のすず";
        case ITEM_DEBUG_ESCAPE: return "にげあしリング";
        default: return "?";
    }
}

static void inventory_ui_clear(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
}

static void inventory_put_field_text(UINT8 x, UINT8 y, UINT8 w, const char *text) BANKED {
    jp_bkg_clear_area(x, y, w, 1u);
    jp_put_bkg_text(x, y, text);
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
    jp_put_bkg_text(x, y, buf);
}

static UINT8 inventory_next_visible(UINT8 start, UINT8 dir) BANKED {
    UINT8 i;
    UINT8 id;

    id = start;
    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        if (dir == 0u) {
            if (id == 0u) id = INVENTORY_ITEM_MAX - 1u;
            else id--;
        } else {
            id++;
            if (id >= INVENTORY_ITEM_MAX) id = 1u;
        }
        if (id != ITEM_NONE && g_inventory_counts[id] != 0u) return id;
    }
    return ITEM_NONE;
}

static UINT8 inventory_row_for_item(UINT8 item_id) BANKED {
    UINT8 row;
    UINT8 id;

    row = 3u;
    for (id = 1u; id < INVENTORY_ITEM_MAX && row < 13u; id++) {
        if (g_inventory_counts[id] == 0u) continue;
        if (id == item_id) return row;
        row++;
    }
    return 0u;
}

static void inventory_draw_items_page(UINT8 cursor_item, const char *message) BANKED {
    UINT8 row;
    UINT8 shown;
    UINT8 id;

    inventory_ui_clear();
    jp_draw_bkg_frame(0u, 0u, 20u, 15u);
    jp_draw_bkg_frame(0u, 15u, 20u, 3u);
    jp_put_bkg_text(1u, 1u, "もちもの");

    shown = 0u;
    row = 3u;
    for (id = 1u; id < INVENTORY_ITEM_MAX && row < 13u; id++) {
        if (g_inventory_counts[id] == 0u) continue;
        jp_put_bkg_text(1u, row, (id == cursor_item) ? ">" : " ");
        inventory_put_field_text(3u, row, 10u, inventory_item_name(id));
        jp_put_bkg_text(14u, row, "x");
        inventory_put_count(16u, row, g_inventory_counts[id]);
        row++;
        shown++;
    }
    if (shown == 0u) jp_put_bkg_text(2u, 6u, "なにもない");
    if (message != 0) inventory_put_field_text(1u, 16u, 18u, message);
}

static void inventory_update_cursor(UINT8 old_item, UINT8 new_item) BANKED {
    UINT8 row;
    row = inventory_row_for_item(old_item);
    if (row != 0u) jp_put_bkg_text(1u, row, " ");
    row = inventory_row_for_item(new_item);
    if (row != 0u) jp_put_bkg_text(1u, row, ">");
}

static void inventory_draw_target_popup(UINT8 item_id, UINT8 slot_cursor) BANKED {

    UINT8 i;
    PartyBattleFighter f;

    jp_draw_bkg_frame(11u, 2u, 9u, 9u);
    inventory_put_field_text(12u, 3u, 7u, inventory_item_name(item_id));
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        party_get_active_fighter(i, &f);
        jp_put_bkg_text(12u, (UINT8)(5u + i), (i == slot_cursor) ? ">" : " ");
        inventory_put_field_text(13u, (UINT8)(5u + i), 5u, f.name);
    }
    inventory_put_field_text(12u, 9u, 7u, "A=使う");
}

static UINT8 inventory_is_field_usable(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HERB:
        case ITEM_POTION:
        case ITEM_MANA_HERB:
        case ITEM_ANTIDOTE:
            return 1u;
        default:
            return 0u;
    }
}

void inventory_menu_show_items_loop(void) BANKED {
    UINT8 keys;
    UINT8 cursor;
    UINT8 old_cursor;
    UINT8 target_slot;
    UINT8 result;
    const char *msg;
    UINT8 popup_redraw;

    cursor = inventory_next_visible(ITEM_NONE, 1u);
    target_slot = 0u;
    msg = "A=つかう B=もどる";
    audio_waitpadup();
    inventory_draw_items_page(cursor, msg);
    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();
        if (keys & J_UP) {
            old_cursor = cursor;
            cursor = inventory_next_visible(cursor, 0u);
            msg = "A=つかう B=もどる";
            inventory_put_field_text(1u, 16u, 18u, msg);
            if (old_cursor != cursor) inventory_update_cursor(old_cursor, cursor);
        } else if (keys & J_DOWN) {
            old_cursor = cursor;
            cursor = inventory_next_visible(cursor, 1u);
            msg = "A=つかう B=もどる";
            inventory_put_field_text(1u, 16u, 18u, msg);
            if (old_cursor != cursor) inventory_update_cursor(old_cursor, cursor);
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            if (cursor == ITEM_NONE) continue;
            if (!inventory_is_field_usable(cursor)) {
                msg = "そのままでは つかえない";
                inventory_put_field_text(1u, 16u, 18u, msg);
                continue;
            }
            inventory_draw_items_page(cursor, "だれに つかう?");
            inventory_draw_target_popup(cursor, target_slot);
            while (1) {
                keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B);
                audio_waitpadup();
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
                    msg = "A=つかう B=もどる";
                    inventory_draw_items_page(cursor, msg);
                    break;
                } else if (keys & J_A) {
                    result = party_use_field_item_on_active(cursor, target_slot);
                    if (result != 0u) {
                        inventory_remove(cursor, 1u);
                        if (inventory_get_count(cursor) == 0u) cursor = inventory_next_visible(cursor, 1u);
                        msg = "つかった!";
                    } else {
                        msg = "こうかが ない";
                    }
                    inventory_draw_items_page(cursor, msg);
                    break;
                }
                if (popup_redraw != 0u) {
                    inventory_draw_target_popup(cursor, target_slot);
                }
            }
        }
    }
}
