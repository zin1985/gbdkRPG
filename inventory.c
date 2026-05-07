#pragma bank 7

#include <gb/gb.h>
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"


static UINT8 g_inventory_counts[INVENTORY_ITEM_MAX];

void inventory_clear(void) {
    UINT8 i;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        g_inventory_counts[i] = 0u;
    }
}

void inventory_seed_defaults(void) {
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
}

UINT8 inventory_get_count(UINT8 item_id) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return g_inventory_counts[item_id];
}

UINT8 inventory_has(UINT8 item_id, UINT8 amount) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    return (g_inventory_counts[item_id] >= amount) ? 1u : 0u;
}

UINT8 inventory_add(UINT8 item_id, UINT8 amount) {
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

UINT8 inventory_remove(UINT8 item_id, UINT8 amount) {
    if (item_id >= INVENTORY_ITEM_MAX) return 0u;
    if (amount == 0u) return 1u;
    if (g_inventory_counts[item_id] < amount) return 0u;

    g_inventory_counts[item_id] = (UINT8)(g_inventory_counts[item_id] - amount);
    return 1u;
}

void inventory_copy_to(UINT8 *dst) {
    UINT8 i;

    if (dst == 0) return;

    for (i = 0u; i < INVENTORY_ITEM_MAX; i++) {
        dst[i] = g_inventory_counts[i];
    }
}

void inventory_copy_from(const UINT8 *src) {
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
        default: return "?";
    }
}

static void inventory_ui_clear(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
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

static void inventory_draw_items_page(UINT8 cursor_item) BANKED {
    UINT8 row;
    UINT8 shown;
    UINT8 id;

    inventory_ui_clear();
    jp_put_bkg_text(1u, 0u, "もちもの");
    jp_put_bkg_text(0u, 1u, "A/Bもどる 上下えらぶ");

    shown = 0u;
    row = 3u;
    for (id = 1u; id < INVENTORY_ITEM_MAX && row < 14u; id++) {
        if (g_inventory_counts[id] == 0u) continue;
        jp_put_bkg_text(0u, row, (id == cursor_item) ? ">" : " ");
        jp_put_bkg_text(1u, row, inventory_item_name(id));
        jp_put_bkg_text(13u, row, "x");
        inventory_put_count(15u, row, g_inventory_counts[id]);
        row++;
        shown++;
    }
    if (shown == 0u) jp_put_bkg_text(1u, 5u, "なにもない");
    jp_put_bkg_text(0u,15u, "やくそう等は次段階で使用");
}

void inventory_menu_show_items_loop(void) BANKED {
    UINT8 keys;
    UINT8 cursor;

    cursor = inventory_next_visible(ITEM_NONE, 1u);
    audio_waitpadup();
    while (1) {
        inventory_draw_items_page(cursor);
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();
        if (keys & J_UP) {
            cursor = inventory_next_visible(cursor, 0u);
        } else if (keys & J_DOWN) {
            cursor = inventory_next_visible(cursor, 1u);
        } else {
            break;
        }
    }
}
