#pragma bank 11

#include <gb/gb.h>
#include "inventory.h"
#include "jpfont.h"
#include "audio.h"
#include "party_runtime.h"
#include "ui_icons.h"


static UINT8 g_inventory_counts[INVENTORY_ITEM_MAX];
static UINT16 g_inventory_money;
/* rpg184: cached visible item list.  The previous menu scanned the whole
 * inventory every time the cursor moved, then redrew the full window.
 * Keep the list in WRAM and refresh it only when counts change. */
static UINT8 g_inventory_visible_ids[INVENTORY_ITEM_MAX];
static UINT8 g_inventory_visible_count_cached;
static char inventory_item_effect_message[16];

#define INVENTORY_GRID_ROWS 11u
#define INVENTORY_GRID_COLS 2u
#define INVENTORY_GRID_PAGE_COUNT ((UINT8)(INVENTORY_GRID_ROWS * INVENTORY_GRID_COLS))


static UINT8 inventory_visible_count(void) BANKED;
static UINT8 inventory_item_at_visible_index(UINT8 index) BANKED;
static UINT8 inventory_clamp_page_top(UINT8 page_top, UINT8 visible_count) BANKED;
static void inventory_draw_items_page(UINT8 cursor_index, UINT8 page_top, const char *message) BANKED;
static void inventory_draw_menu_shell(void) BANKED;
static void inventory_clear_list_area(void) BANKED;
static void inventory_draw_cursor_only(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED;
static void inventory_draw_page_counter(UINT8 cursor_index, UINT8 visible_count) BANKED;
static UINT8 inventory_cursor_down(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED;
static UINT8 inventory_cursor_up(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED;
static UINT8 inventory_wait_menu_keys(UINT8 mask) BANKED;
static const char *inventory_make_item_result_message(UINT8 item_id) BANKED;
static UINT8 inventory_effect_is_growth(UINT8 item_id) BANKED;
static void inventory_draw_target_popup(UINT8 item_id, UINT8 slot_cursor) BANKED;

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



UINT8 inventory_battle_use_auto(UINT8 active_slot) BANKED {
    UINT8 item_id;
    item_id = ITEM_NONE;
    if (inventory_has(ITEM_HERB, 1u)) item_id = ITEM_HERB;
    else if (inventory_has(ITEM_POTION, 1u)) item_id = ITEM_POTION;
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
    inventory_rebuild_battle_visible_cache();
    visible_count = inventory_visible_count();
    cursor_index = 0u;
    page_top = 0u;
    target_slot = active_slot;
    msg = "A=使う B=戻る";
    inventory_draw_menu_shell();
    inventory_draw_items_page(cursor_index, page_top, msg);

    while (1) {
        visible_count = inventory_visible_count();
        if (visible_count == 0u) {
            inventory_draw_items_page(0u, 0u, "使える道具なし");
            keys = inventory_wait_menu_keys(J_B | J_START | J_A);
            (void)keys;
            return 0u;
        }
        if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
        page_top = inventory_clamp_page_top(page_top, visible_count);
        old_cursor = cursor_index;
        old_page = page_top;

        keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START);
        if (keys & (J_B | J_START)) return 0u;
        if (keys & J_UP) cursor_index = inventory_cursor_up(cursor_index, &page_top, visible_count);
        else if (keys & J_DOWN) cursor_index = inventory_cursor_down(cursor_index, &page_top, visible_count);
        else if (keys & J_LEFT) {
            if ((cursor_index & 1u) && cursor_index > page_top) cursor_index--;
        } else if (keys & J_RIGHT) {
            if (((cursor_index & 1u) == 0u) && ((UINT8)(cursor_index + 1u) < visible_count) && ((UINT8)(cursor_index + 1u) < (UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT))) cursor_index++;
        } else if (keys & J_A) {
            item_id = inventory_item_at_visible_index(cursor_index);
            inventory_draw_items_page(cursor_index, page_top, "だれに つかう?");
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
                    msg = "A=使う B=戻る";
                    inventory_draw_items_page(cursor_index, page_top, msg);
                    break;
                } else if (keys & J_A) {
                    result = party_use_field_item_on_active(item_id, target_slot);
                    if (result != 0u) {
                        inventory_remove(item_id, 1u);
                        msg = inventory_make_item_result_message(item_id);
                        return 2u;
                    }
                    msg = "こうかが ない";
                    inventory_draw_items_page(cursor_index, page_top, msg);
                    break;
                }
                if (popup_redraw != 0u) inventory_draw_target_popup(item_id, target_slot);
            }
            continue;
        }

        if (old_cursor != cursor_index || old_page != page_top) {
            if (old_page != page_top) inventory_draw_items_page(cursor_index, page_top, msg);
            else {
                inventory_draw_cursor_only(old_cursor, page_top, 0u);
                inventory_draw_cursor_only(cursor_index, page_top, 1u);
                inventory_draw_page_counter(cursor_index, visible_count);
            }
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
        case ITEM_DEBUG_ESCAPE: return "はやあしリング";
        case ITEM_MEDICINE: return "きずぐすり";
        case ITEM_HIGH_POTION: return "上ポーション";
        case ITEM_ELIXIR: return "エリクサ";
        case ITEM_MANA_DROP: return "まほう水";
        case ITEM_MANA_BOTTLE: return "まほうびん";
        case ITEM_FULL_HERB: return "全快草";
        case ITEM_PANACEA: return "ばんのう薬";
        case ITEM_WAKE_HERB: return "めざめ草";
        case ITEM_EYE_DROP: return "めぐすり";
        case ITEM_SMOKE_BOMB: return "けむり玉";
        case ITEM_BARRIER_SEED: return "まもり種";
        case ITEM_POWER_SEED: return "ちから種";
        case ITEM_GUARD_SEED: return "かたい種";
        case ITEM_SPEED_SEED: return "はやさ種";
        case ITEM_FOCUS_TEA: return "集中茶";
        case ITEM_MORALE_MEAT: return "戦意肉";
        case ITEM_ICE_BOMB: return "氷ばくだん";
        case ITEM_FIRE_BOMB: return "火ばくだん";
        case ITEM_THUNDER_BOMB: return "雷ばくだん";
        case ITEM_LIGHT_BOTTLE: return "光のびん";
        case ITEM_DARK_BOTTLE: return "闇のびん";
        case ITEM_RETURN_FEATHER: return "帰還羽";
        case ITEM_TENT: return "テント";
        case ITEM_REVIVE_STONE: return "命の石";
        case ITEM_COPPER_SWORD: return "銅のけん";
        case ITEM_STEEL_SWORD: return "鋼のけん";
        case ITEM_SILVER_SWORD: return "銀のけん";
        case ITEM_FLAME_SWORD: return "炎のけん";
        case ITEM_ICE_SWORD: return "氷のけん";
        case ITEM_DRAGON_SWORD: return "竜のけん";
        case ITEM_WILLOW_STAFF: return "柳のつえ";
        case ITEM_SAGE_STAFF: return "賢者つえ";
        case ITEM_THUNDER_STAFF: return "雷のつえ";
        case ITEM_MOON_STAFF: return "月のつえ";
        case ITEM_SHADOW_STAFF: return "影のつえ";
        case ITEM_LONG_BOW: return "長いゆみ";
        case ITEM_STEEL_BOW: return "鋼のゆみ";
        case ITEM_HUNTER_BOW: return "狩人ゆみ";
        case ITEM_THUNDER_BOW: return "雷のゆみ";
        case ITEM_STAR_BOW: return "星のゆみ";
        case ITEM_BRONZE_SPEAR: return "青銅のやり";
        case ITEM_STEEL_SPEAR: return "鋼のやり";
        case ITEM_DRAGON_SPEAR: return "竜槍";
        case ITEM_HAND_AXE: return "手おの";
        case ITEM_BATTLE_AXE: return "戦おの";
        case ITEM_GREAT_AXE: return "大おの";
        case ITEM_IRON_GLOVES: return "鉄手甲";
        case ITEM_BEAST_CLAWS: return "獣の爪";
        case ITEM_POWER_KNUCKLE: return "力手甲";
        case ITEM_MONK_FIST: return "僧の拳";
        case ITEM_ALCHEMY_KIT: return "錬金具";
        case ITEM_TRAP_TOOL: return "罠道具";
        case ITEM_BOMB_TOOL: return "爆弾具";
        case ITEM_GEAR_TOOL: return "歯車具";
        case ITEM_TRAVEL_CLOTH: return "旅の服";
        case ITEM_MAGE_ROBE: return "魔法ローブ";
        case ITEM_BATTLE_CLOTH: return "戦い服";
        case ITEM_CHAIN_MAIL: return "鎖かたびら";
        case ITEM_STEEL_ARMOR: return "鋼よろい";
        case ITEM_SILVER_ARMOR: return "銀よろい";
        case ITEM_DRAGON_ARMOR: return "竜よろい";
        case ITEM_QUICK_BOOTS: return "早足くつ";
        case ITEM_POWER_RING: return "力の指輪";
        case ITEM_GUARD_RING: return "守り指輪";
        case ITEM_FOCUS_RING: return "集中指輪";
        case ITEM_LUCKY_COIN: return "幸運コイン";
        case ITEM_DRAGON_SCALE: return "竜のうろこ";
        case ITEM_SLEEP_GUARD: return "眠り守り";
        case ITEM_POISON_GUARD: return "毒守り";
        case ITEM_LIGHT_MEDAL: return "光メダル";
        case ITEM_DARK_MEDAL: return "闇メダル";
        case ITEM_ITIL_SWORD: return "アイティルの剣";
        case ITEM_ITIL_ARMOR: return "アイティルの鎧";
        case ITEM_ITIL_CREST: return "アイティルの紋章";
        default: return "?";
    }
}


const char *inventory_get_item_name(UINT8 item_id) BANKED {
    return inventory_item_name(item_id);
}
static const char *inventory_item_short_name(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HIGH_POTION: return "上ポー";
        case ITEM_MANA_BOTTLE: return "魔びん";
        case ITEM_DEBUG_NO_ENCOUNT: return "退魔鈴";
        case ITEM_DEBUG_ESCAPE: return "早足輪";
        case ITEM_RETURN_FEATHER: return "帰還羽";
        case ITEM_REVIVE_STONE: return "命の石";
        case ITEM_BRONZE_SPEAR: return "青銅槍";
        case ITEM_STEEL_SPEAR: return "鋼槍";
        case ITEM_HAND_AXE: return "手おの";
        case ITEM_BATTLE_AXE: return "戦おの";
        case ITEM_POWER_KNUCKLE: return "力手甲";
        case ITEM_DRAGON_SCALE: return "竜うろ";
        default: return inventory_item_name(item_id);
    }
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


#define INVENTORY_REPEAT_DELAY 12u
#define INVENTORY_REPEAT_RATE   4u

static UINT8 inventory_wait_menu_keys(UINT8 mask) BANKED {
    static UINT8 last_keys;
    static UINT8 repeat_count;
    UINT8 keys;
    UINT8 pressed;
    UINT8 dirs;

    while (1) {
        keys = (UINT8)(joypad() & mask);
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
    if (page_top >= INVENTORY_GRID_PAGE_COUNT) jp_put_bkg_text(18u, 2u, "▲");
    else jp_put_bkg_text(18u, 2u, " ");
    if ((UINT8)(page_top + INVENTORY_GRID_PAGE_COUNT) < visible_count) jp_put_bkg_text(18u, 14u, "▼");
    else jp_put_bkg_text(18u, 14u, " ");
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

    if (visible_count <= INVENTORY_GRID_PAGE_COUNT) return 0u;
    max_top = (UINT8)(visible_count - 1u);
    max_top = (UINT8)(max_top / INVENTORY_GRID_PAGE_COUNT);
    max_top = (UINT8)(max_top * INVENTORY_GRID_PAGE_COUNT);
    if (page_top > max_top) page_top = max_top;
    page_top = (UINT8)((page_top / INVENTORY_GRID_PAGE_COUNT) * INVENTORY_GRID_PAGE_COUNT);
    return page_top;
}

static void inventory_draw_grid_entry(UINT8 visible_index, UINT8 cursor_index, UINT8 row, UINT8 col) BANKED {
    UINT8 id;
    UINT8 x;

    id = inventory_item_at_visible_index(visible_index);
    x = (col == 0u) ? 1u : 10u;
    if (id == ITEM_NONE) {
        jp_bkg_clear_area(x, row, 9u, 1u);
        return;
    }
    jp_put_bkg_text(x, row, (visible_index == cursor_index) ? ">" : " ");
    ui_put_icon((UINT8)(x + 1u), row, ui_icon_tile_for_item(id));
    inventory_put_field_text((UINT8)(x + 2u), row, 5u, inventory_item_short_name(id));
    inventory_put_count((UINT8)(x + 7u), row, g_inventory_counts[id]);
}

static void inventory_draw_cursor_only(UINT8 visible_index, UINT8 page_top, UINT8 enabled) BANKED {
    UINT8 offset;
    UINT8 x;
    UINT8 y;

    if (visible_index < page_top) return;
    offset = (UINT8)(visible_index - page_top);
    if (offset >= INVENTORY_GRID_PAGE_COUNT) return;
    x = (offset & 1u) ? 10u : 1u;
    y = (UINT8)(3u + (offset >> 1));
    jp_put_bkg_text(x, y, enabled ? ">" : " ");
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
    UINT8 idx;
    UINT8 visible_count;

    visible_count = inventory_visible_count();
    page_top = inventory_clamp_page_top(page_top, visible_count);
    inventory_clear_list_area();
    jp_put_bkg_text(1u, 1u, "もちもの");

    if (visible_count == 0u) {
        jp_put_bkg_text(2u, 6u, "なにもない");
    } else {
        row = 3u;
        idx = page_top;
        for (r = 0u; r < INVENTORY_GRID_ROWS; r++, row++) {
            inventory_draw_grid_entry(idx, cursor_index, row, 0u);
            idx++;
            inventory_draw_grid_entry(idx, cursor_index, row, 1u);
            idx++;
        }
        inventory_draw_page_counter(cursor_index, visible_count);
        inventory_draw_page_arrows(page_top, visible_count);
    }
    if (message != 0) inventory_put_field_text(1u, 16u, 18u, message);
}

static UINT8 inventory_cursor_down(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED {
    UINT8 offset;
    UINT8 col;
    UINT8 next_page;
    UINT8 next;

    if (visible_count == 0u) return 0u;
    offset = (UINT8)(cursor_index - *page_top);
    col = (UINT8)(cursor_index & 1u);
    if ((offset < (INVENTORY_GRID_PAGE_COUNT - 2u)) && ((UINT8)(cursor_index + 2u) < visible_count)) {
        return (UINT8)(cursor_index + 2u);
    }
    next_page = (UINT8)(*page_top + INVENTORY_GRID_PAGE_COUNT);
    if (next_page < visible_count) {
        *page_top = next_page;
        next = (UINT8)(next_page + col);
        if (next >= visible_count) next = (UINT8)(visible_count - 1u);
        return next;
    }
    return cursor_index;
}

static UINT8 inventory_cursor_up(UINT8 cursor_index, UINT8 *page_top, UINT8 visible_count) BANKED {
    UINT8 offset;
    UINT8 col;
    UINT8 prev_page;
    UINT8 next;

    if (visible_count == 0u) return 0u;
    offset = (UINT8)(cursor_index - *page_top);
    col = (UINT8)(cursor_index & 1u);
    if (offset >= 2u) return (UINT8)(cursor_index - 2u);
    if (*page_top >= INVENTORY_GRID_PAGE_COUNT) {
        prev_page = (UINT8)(*page_top - INVENTORY_GRID_PAGE_COUNT);
        *page_top = prev_page;
        next = (UINT8)(prev_page + ((INVENTORY_GRID_ROWS - 1u) * 2u) + col);
        if (next >= visible_count) next = (UINT8)(visible_count - 1u);
        return next;
    }
    return cursor_index;
}

static UINT8 inventory_is_field_usable(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_HERB:
        case ITEM_POTION:
        case ITEM_MANA_HERB:
        case ITEM_ANTIDOTE:
        case ITEM_MEDICINE:
        case ITEM_HIGH_POTION:
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
            return "あがった";
        }
        (void)kind;
        inventory_put_field_text(1u, 16u, 18u, inventory_item_effect_message);
        return "かいふく";
    }
    return "つかった!";
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

    ui_icons_load();
    inventory_rebuild_visible_cache();
    visible_count = inventory_visible_count();
    cursor_index = 0u;
    page_top = 0u;
    target_slot = 0u;
    msg = "A=つかう B=もどる";
    audio_waitpadup();
    inventory_draw_menu_shell();
    inventory_draw_items_page(cursor_index, page_top, msg);
    while (1) {
        visible_count = inventory_visible_count();
        if (visible_count == 0u) cursor_index = 0u;
        else if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
        page_top = inventory_clamp_page_top(page_top, visible_count);

        keys = inventory_wait_menu_keys(J_UP | J_DOWN | J_LEFT | J_RIGHT | J_A | J_B | J_START);
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
        } else if (keys & (J_B | J_START)) {
            break;
        } else if (keys & J_A) {
            if (visible_count == 0u) continue;
            cursor_item = inventory_item_at_visible_index(cursor_index);
            if (cursor_item == ITEM_NONE) continue;
            if (!inventory_is_field_usable(cursor_item)) {
                msg = "そのままでは つかえない";
                inventory_draw_items_page(cursor_index, page_top, msg);
                continue;
            }
            inventory_draw_items_page(cursor_index, page_top, "だれに つかう?");
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
                    msg = "A=つかう B=もどる";
                    inventory_draw_items_page(cursor_index, page_top, msg);
                    break;
                } else if (keys & J_A) {
                    result = party_use_field_item_on_active(cursor_item, target_slot);
                    if (result != 0u) {
                        inventory_remove(cursor_item, 1u);
                        inventory_rebuild_visible_cache();
                        visible_count = inventory_visible_count();
                        if (visible_count == 0u) cursor_index = 0u;
                        else if (cursor_index >= visible_count) cursor_index = (UINT8)(visible_count - 1u);
                        page_top = inventory_clamp_page_top(page_top, visible_count);
                        msg = inventory_make_item_result_message(cursor_item);
                    } else {
                        msg = "こうかが ない";
                    }
                    inventory_draw_items_page(cursor_index, page_top, msg);
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
                inventory_draw_items_page(cursor_index, page_top, msg);
            } else {
                inventory_draw_cursor_only(old_cursor, page_top, 0u);
                inventory_draw_cursor_only(cursor_index, page_top, 1u);
                inventory_draw_page_counter(cursor_index, visible_count);
            }
        }
    }
}

