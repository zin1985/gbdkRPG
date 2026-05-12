#pragma bank 5

#include <gb/gb.h>
#include "audio.h"
#include "jpfont.h"
#include "party_runtime.h"
#include "battle_skill_runtime.h"
#include "skill_defs.h"

BANKREF(battle_skill_runtime)

#define SKILL_KIND_DAMAGE 0u
#define SKILL_KIND_HEAL   1u

/* main.c owns the actual learned skill list.  This runtime only draws and selects it. */
UINT8 player_get_skill_slot(UINT8 slot);
UINT8 player_get_skill_count(void);
UINT8 player_get_skill_at(UINT8 index);

static UINT8 skill_cursor;
static UINT8 skill_page_top;
static UINT8 skill_count;
static UINT8 skill_ids[PLAYER_SKILL_SLOT_COUNT];
static UINT8 selected_skill;
static UINT8 ally_cursor;
static UINT8 list_magic_mode;

static UINT8 runtime_is_magic(UINT8 skill_id) {
    return battle_skill_runtime_is_magic(skill_id);
}

static const char *runtime_skill_name(UINT8 skill_id) {
    switch (skill_id) {
        case SKILL_POWER_STRIKE: return "パワー";
        case SKILL_HEAL_SIMPLE: return "ヒール";
        case SKILL_FIRE: return "ファイア";
        case SKILL_GUARD_BREAK: return "ブレイク";
        case SKILL_SLASH: return "スラッシュ";
        case SKILL_DOUBLE_SLASH: return "ダブル";
        case SKILL_CROSS_SLASH: return "クロス";
        case SKILL_FLAME_EDGE: return "フレイム";
        case SKILL_ICE_EDGE: return "アイス";
        case SKILL_DRAGON_REND: return "ドラゴン";
        case SKILL_HEAVY_BLOW: return "ヘビー";
        case SKILL_ARMOR_BREAK: return "アーマー";
        case SKILL_WIND_CUT: return "ウィンド";
        case SKILL_FINAL_BLADE: return "ファイナル";
        case SKILL_STAFF_HIT: return "スタッフ";
        case SKILL_MIND_SPIKE: return "マインド";
        case SKILL_HOLY_LIGHT: return "ホーリー";
        case SKILL_MANA_BURST: return "マナ";
        case SKILL_SLEEP_MIST: return "スリープ";
        case SKILL_STAR_PRAYER: return "スター";
        case SKILL_ARROW_SHOT: return "アロー";
        case SKILL_DOUBLE_ARROW: return "ツイン";
        case SKILL_EAGLE_EYE: return "イーグル";
        case SKILL_PIERCING_ARROW: return "ピアス";
        case SKILL_THUNDER_ARROW: return "サンダー";
        case SKILL_RAIN_ARROW: return "レイン";
        case SKILL_PUNCH: return "パンチ";
        case SKILL_IRON_FIST: return "アイアン";
        case SKILL_COUNTER_FIST: return "カウンタ";
        case SKILL_TIGER_CLAW: return "タイガー";
        case SKILL_DRAGON_FIST: return "ドラゴン";
        case SKILL_SOUL_PALM: return "ソウル";
        case SKILL_TRAP_SET: return "トラップ";
        case SKILL_BOMB_THROW: return "ボム";
        case SKILL_GEAR_CUTTER: return "ギア";
        case SKILL_FLASH_BOMB: return "フラッシュ";
        case SKILL_POISON_TRAP: return "ポイズン";
        case SKILL_GRAVITY_NET: return "グラビティ";
        case SKILL_MAGIC_FLAME: return "フレア";
        case SKILL_MAGIC_BLIZZARD: return "ブリザド";
        case SKILL_MAGIC_THUNDER: return "サンダー";
        case SKILL_MAGIC_AQUA: return "アクア";
        case SKILL_MAGIC_WIND: return "エアロ";
        case SKILL_MAGIC_STONE: return "ストーン";
        case SKILL_MAGIC_SHINE: return "シャイン";
        case SKILL_MAGIC_DARK: return "ダーク";
        case SKILL_MAGIC_HEAL_PLUS: return "ハイヒール";
        case SKILL_MAGIC_REVIVE: return "リバイブ";
        case SKILL_MAGIC_BARRIER: return "バリア";
        case SKILL_MAGIC_METEOR: return "メテオ";
        default: return "なし";
    }
}

static UINT8 runtime_skill_kind(UINT8 skill_id) {
    return battle_skill_runtime_kind(skill_id);
}

static UINT8 add_skill_choice(UINT8 skill_id) {
    UINT8 i;
    if (skill_id == SKILL_NONE) return 0u;
    for (i = 0u; i < skill_count; i++) {
        if (skill_ids[i] == skill_id) return 0u;
    }
    if (skill_count >= PLAYER_SKILL_SLOT_COUNT) return 0u;
    skill_ids[skill_count] = skill_id;
    skill_count++;
    return 1u;
}

static void clamp_skill_page(void) {
    if (skill_cursor < skill_page_top) skill_page_top = skill_cursor;
    if (skill_cursor >= (UINT8)(skill_page_top + PLAYER_SKILL_VISIBLE_ROWS)) {
        skill_page_top = (UINT8)(skill_cursor - (PLAYER_SKILL_VISIBLE_ROWS - 1u));
    }
}

static void draw_skill_window(void) {
    UINT8 row;
    UINT8 idx;
    jp_draw_bkg_frame(8u, 11u, 12u, 7u);
    jp_bkg_clear_area(9u, 12u, 10u, 5u);
    for (row = 0u; row < PLAYER_SKILL_VISIBLE_ROWS; row++) {
        idx = (UINT8)(skill_page_top + row);
        if (idx < skill_count) {
            jp_put_bkg_text(9u, (UINT8)(12u + row), (idx == skill_cursor) ? ">" : " ");
            jp_put_bkg_text(10u, (UINT8)(12u + row), runtime_skill_name(skill_ids[idx]));
        }
    }
    jp_put_bkg_text(9u, 16u, "Aけってい Bもどる");
}

static void build_skill_list(UINT8 party_turn_slot, UINT8 magic_mode) {
    UINT8 member_id;
    UINT8 i;
    UINT8 count;
    UINT8 sid;

    skill_count = 0u;
    list_magic_mode = magic_mode;
    for (i = 0u; i < PLAYER_SKILL_SLOT_COUNT; i++) skill_ids[i] = SKILL_NONE;

    member_id = party_get_active_member_id(party_turn_slot);
    if (magic_mode) {
        if (member_id == PARTY_MEMBER_PRIEST) {
            add_skill_choice(SKILL_HEAL_SIMPLE);
            add_skill_choice(SKILL_MAGIC_HEAL_PLUS);
            add_skill_choice(SKILL_MAGIC_BARRIER);
        }
        if (member_id == PARTY_MEMBER_MAGE) {
            add_skill_choice(SKILL_FIRE);
            add_skill_choice(SKILL_MAGIC_FLAME);
            add_skill_choice(SKILL_MAGIC_BLIZZARD);
            add_skill_choice(SKILL_MAGIC_THUNDER);
        }
        count = player_get_skill_count();
        for (i = 0u; i < count; i++) {
            sid = player_get_skill_at(i);
            if (runtime_is_magic(sid)) add_skill_choice(sid);
        }
    } else {
        count = player_get_skill_count();
        for (i = 0u; i < count; i++) {
            sid = player_get_skill_at(i);
            if (!runtime_is_magic(sid)) add_skill_choice(sid);
        }
        if (skill_count == 0u) add_skill_choice(SKILL_POWER_STRIKE);
    }
    skill_cursor = 0u;
    skill_page_top = 0u;
}

void battle_skill_runtime_start(UINT8 party_turn_slot) BANKED {
    build_skill_list(party_turn_slot, 0u);
    draw_skill_window();
}

void battle_skill_runtime_start_magic(UINT8 party_turn_slot) BANKED {
    build_skill_list(party_turn_slot, 1u);
    draw_skill_window();
}

UINT8 battle_skill_runtime_update(UINT8 *skill_id) BANKED {
    UINT8 keys;
    keys = joypad();
    if (keys & J_UP) {
        if (skill_cursor == 0u) skill_cursor = (UINT8)(skill_count - 1u);
        else skill_cursor--;
        clamp_skill_page();
        draw_skill_window();
        audio_waitpadup();
    } else if (keys & J_DOWN) {
        skill_cursor++;
        if (skill_cursor >= skill_count) skill_cursor = 0u;
        clamp_skill_page();
        draw_skill_window();
        audio_waitpadup();
    } else if (keys & J_B) {
        audio_waitpadup();
        return BATTLE_SKILL_EVENT_CANCEL;
    } else if (keys & J_A) {
        audio_waitpadup();
        *skill_id = skill_ids[skill_cursor];
        if (runtime_skill_kind(*skill_id) == SKILL_KIND_HEAL) {
            return BATTLE_SKILL_EVENT_ALLY_SKILL;
        }
        return BATTLE_SKILL_EVENT_ENEMY_SKILL;
    }
    return BATTLE_SKILL_EVENT_NONE;
}

static void draw_ally_window(void) {
    UINT8 i;
    jp_draw_bkg_frame(9u, 12u, 11u, 6u);
    jp_bkg_clear_area(10u, 13u, 9u, 4u);
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        jp_put_bkg_text(10u, (UINT8)(13u + i), (i == ally_cursor) ? ">" : " ");
        party_put_active_name_battle(i, 11u, (UINT8)(13u + i));
    }
}

static void select_first_alive_ally(void) {
    UINT8 i;
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        if (party_get_active_hp(i) > 0u) {
            ally_cursor = i;
            return;
        }
    }
    ally_cursor = 0u;
}

void battle_skill_runtime_start_ally(UINT8 skill_id) BANKED {
    selected_skill = skill_id;
    select_first_alive_ally();
    draw_ally_window();
}

UINT8 battle_skill_runtime_update_ally(UINT8 *skill_id, UINT8 *ally_slot) BANKED {
    UINT8 keys;
    keys = joypad();
    if (keys & J_UP) {
        if (ally_cursor == 0u) ally_cursor = PARTY_ACTIVE_COUNT - 1u;
        else ally_cursor--;
        draw_ally_window();
        audio_waitpadup();
    } else if (keys & J_DOWN) {
        ally_cursor++;
        if (ally_cursor >= PARTY_ACTIVE_COUNT) ally_cursor = 0u;
        draw_ally_window();
        audio_waitpadup();
    } else if (keys & J_B) {
        audio_waitpadup();
        return BATTLE_SKILL_EVENT_CANCEL;
    } else if (keys & J_A) {
        audio_waitpadup();
        *skill_id = selected_skill;
        *ally_slot = ally_cursor;
        return BATTLE_SKILL_EVENT_ALLY_SKILL;
    }
    return BATTLE_SKILL_EVENT_NONE;
}
