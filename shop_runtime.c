#pragma bank 5

#include <gb/gb.h>
#include "shop_runtime.h"
#include "jpfont.h"
#include "audio.h"
#include "inventory.h"
#include "party_runtime.h"
#include "field_feature_runtime.h"
#include "messages.h"

BANKREF(shop_runtime_bank)

static UINT8 shop_town_pot_opened;

#define SHOP_ITEM_COUNT 3u

static const UINT8 item_shop_ids[SHOP_ITEM_COUNT] = {
    ITEM_POTION,
    ITEM_MANA_HERB,
    ITEM_ANTIDOTE
};

static const UINT8 item_shop_prices[SHOP_ITEM_COUNT] = {
    20u,
    30u,
    8u
};

static const UINT8 equip_shop_ids[SHOP_ITEM_COUNT] = {
    ITEM_IRON_SWORD,
    ITEM_LEATHER_ARMOR,
    ITEM_DEBUG_ESCAPE
};

static const UINT8 equip_shop_prices[SHOP_ITEM_COUNT] = {
    60u,
    45u,
    1u
};

static void shop_clear(void) BANKED {
    jp_bkg_clear_area(0u, 0u, 20u, 18u);
    jp_draw_bkg_frame(0u, 0u, 20u, 18u);
}

static void shop_u16_to_dec(UINT16 value, char *out) BANKED {
    UINT8 i;
    UINT8 started;
    UINT16 div;

    i = 0u;
    started = 0u;
    div = 10000u;
    while (div > 0u) {
        UINT8 digit = 0u;
        while (value >= div) {
            value = (UINT16)(value - div);
            digit++;
        }
        if (digit != 0u || started || div == 1u) {
            out[i++] = (char)('0' + digit);
            started = 1u;
        }
        div /= 10u;
    }
    out[i] = '\0';
}

static void shop_draw_money(UINT8 y) BANKED {
    char buf[8];
    jp_bkg_clear_area(1u, y, 18u, 1u);
    jp_put_bkg_text(1u, y, "G ");
    shop_u16_to_dec(inventory_get_money(), buf);
    jp_put_bkg_text(3u, y, buf);
}

static const char *shop_item_name(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_POTION: return "ポーション";
        case ITEM_MANA_HERB: return "まほう草";
        case ITEM_ANTIDOTE: return "どくけし";
        case ITEM_IRON_SWORD: return "鉄のけん";
        case ITEM_LEATHER_ARMOR: return "かわよろい";
        case ITEM_DEBUG_ESCAPE: return "にげあしリング";
        default: return "?";
    }
}

static void shop_draw_cursor(UINT8 cursor, UINT8 count) BANKED {
    UINT8 i;
    for (i = 0u; i < count; i++) {
        jp_put_bkg_text(1u, (UINT8)(4u + i), (cursor == i) ? ">" : " ");
    }
}

static void shop_draw_list(UINT8 kind, UINT8 cursor) BANKED {
    const UINT8 *ids;
    const UINT8 *prices;
    UINT8 i;
    char buf[8];

    ids = (kind == SHOP_KIND_ITEM) ? item_shop_ids : equip_shop_ids;
    prices = (kind == SHOP_KIND_ITEM) ? item_shop_prices : equip_shop_prices;

    shop_clear();
    jp_put_bkg_text(1u, 1u, (kind == SHOP_KIND_ITEM) ? "どうぐや" : "ぶぐや");
    shop_draw_money(2u);

    for (i = 0u; i < SHOP_ITEM_COUNT; i++) {
        jp_put_bkg_text(2u, (UINT8)(4u + i), shop_item_name(ids[i]));
        shop_u16_to_dec(prices[i], buf);
        jp_put_bkg_text(14u, (UINT8)(4u + i), buf);
        jp_put_bkg_text(17u, (UINT8)(4u + i), "G");
    }
    jp_put_bkg_text(1u, 16u, "A かう  B やめる");
    shop_draw_cursor(cursor, SHOP_ITEM_COUNT);
}

static void shop_buy_loop(UINT8 kind) BANKED {
    UINT8 cursor;
    UINT8 keys;
    const UINT8 *ids;
    const UINT8 *prices;

    cursor = 0u;
    ids = (kind == SHOP_KIND_ITEM) ? item_shop_ids : equip_shop_ids;
    prices = (kind == SHOP_KIND_ITEM) ? item_shop_prices : equip_shop_prices;

    shop_draw_list(kind, cursor);
    while (1) {
        keys = audio_waitpad(J_UP | J_DOWN | J_A | J_B | J_START);
        audio_waitpadup();

        if (keys & J_UP) {
            if (cursor == 0u) cursor = SHOP_ITEM_COUNT - 1u;
            else cursor--;
            shop_draw_cursor(cursor, SHOP_ITEM_COUNT);
        } else if (keys & J_DOWN) {
            cursor++;
            if (cursor >= SHOP_ITEM_COUNT) cursor = 0u;
            shop_draw_cursor(cursor, SHOP_ITEM_COUNT);
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            jp_bkg_clear_area(1u, 13u, 18u, 2u);
            if (inventory_spend_money(prices[cursor])) {
                inventory_add(ids[cursor], 1u);
                jp_put_bkg_text(1u, 13u, "かった！");
            } else {
                jp_put_bkg_text(1u, 13u, "Gが たりない");
            }
            shop_draw_money(2u);
        }
    }
}

static void shop_inn_loop(void) BANKED {
    UINT8 keys;

    shop_clear();
    jp_put_bkg_text(1u, 1u, "やどや");
    shop_draw_money(2u);
    jp_put_bkg_text(2u, 5u, "10Gで とまりますか");
    jp_put_bkg_text(1u, 16u, "A とまる  B やめる");

    keys = audio_waitpad(J_A | J_B | J_START);
    audio_waitpadup();
    if (keys & J_A) {
        jp_bkg_clear_area(1u, 12u, 18u, 2u);
        if (inventory_spend_money(10u)) {
            party_heal_all_active();
            jp_put_bkg_text(1u, 12u, "みんな かいふくした");
        } else {
            jp_put_bkg_text(1u, 12u, "Gが たりない");
        }
        shop_draw_money(2u);
        audio_waitpad(J_A | J_B | J_START);
        audio_waitpadup();
    }
}

void shop_runtime_open(UINT8 shop_kind) BANKED {
    if (shop_kind == SHOP_KIND_INN) {
        shop_inn_loop();
    } else if (shop_kind == SHOP_KIND_ITEM || shop_kind == SHOP_KIND_EQUIP) {
        shop_buy_loop(shop_kind);
    }
}

UINT8 shop_runtime_handle_event(UINT8 event_id) BANKED {
    if (event_id == MAP_EVENT_SHOP_INN) {
        shop_runtime_open(SHOP_KIND_INN);
        return 1u;
    }
    if (event_id == MAP_EVENT_SHOP_ITEM) {
        shop_runtime_open(SHOP_KIND_ITEM);
        return 1u;
    }
    if (event_id == MAP_EVENT_SHOP_EQUIP) {
        shop_runtime_open(SHOP_KIND_EQUIP);
        return 1u;
    }
    if (event_id == MAP_EVENT_POT) {
        if (shop_town_pot_opened) {
            message_show(MSG_TREASURE_EMPTY);
        } else {
            shop_town_pot_opened = 1u;
            inventory_add(ITEM_HERB, 1u);
            message_show(MSG_POT_FOUND);
        }
    }
    return 0u;
}

UINT8 shop_runtime_get_pot_opened(void) BANKED {
    return shop_town_pot_opened;
}

void shop_runtime_set_pot_opened(UINT8 opened) BANKED {
    shop_town_pot_opened = opened ? 1u : 0u;
}
