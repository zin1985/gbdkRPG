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

const char *battle_skill_runtime_name(UINT8 skill_id) BANKED {
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
        case SKILL_SWORD_MOON: return "ムーン";
        case SKILL_SWORD_COMET: return "コメット";
        case SKILL_STAFF_NOVA: return "ノヴァ";
        case SKILL_STAFF_SPIRIT: return "スピリット";
        case SKILL_BOW_STORM: return "ストーム";
        case SKILL_BOW_STARFALL: return "スター";
        case SKILL_FIST_METEOR: return "メテオ拳";
        case SKILL_FIST_AURA: return "オーラ";
        case SKILL_TOOL_RAIL: return "レール";
        case SKILL_TOOL_NOVA_BOMB: return "ノヴァボム";
        case SKILL_MAGIC_FREEZE: return "フリーズ";
        case SKILL_MAGIC_PLASMA: return "プラズマ";
        case SKILL_MAGIC_TORNADO: return "トルネド";
        case SKILL_MAGIC_GAIA: return "ガイア";
        case SKILL_MAGIC_HOLY_RAY: return "ホーリィ";
        case SKILL_MAGIC_ABYSS: return "アビス";
        case SKILL_MAGIC_CURE_ALL: return "キュアオル";
        case SKILL_MAGIC_ACCEL: return "アクセル";
        case SKILL_MAGIC_FLARE: return "フレアII";
        case SKILL_MAGIC_GRAND_CROSS: return "グランド";
        case SKILL_SWORD_SKY: return "スカイ";
        case SKILL_SWORD_VOID: return "ヴォイド";
        case SKILL_STAFF_ARCANA: return "アルカナ";
        case SKILL_BOW_COMET: return "コメット矢";
        case SKILL_FIST_GODHAND: return "ゴッド手";
        case SKILL_TOOL_ZERO: return "ゼロ装置";
        case SKILL_MAGIC_ZERO: return "ゼロ";
        case SKILL_MAGIC_LUNA: return "ルナ";
        case SKILL_MAGIC_PHOTON: return "フォトン";
        case SKILL_MAGIC_ULTIMA: return "アルテマ";
        case SKILL_SWORD_BRAVE_EDGE: return "ブレイブ";
        case SKILL_SWORD_WAVE: return "なぎ";
        case SKILL_SWORD_STORM: return "ストーム";
        case SKILL_SWORD_RAIN: return "ソードレイン";
        case SKILL_AXE_TOMAHAWK: return "トマホク";
        case SKILL_AXE_CLEAVE: return "クリーブ";
        case SKILL_AXE_GIGANT: return "ギガント";
        case SKILL_AXE_QUAKE: return "クェイク";
        case SKILL_LANCE_THRUST: return "スラスト";
        case SKILL_LANCE_PIERCE: return "ピアース";
        case SKILL_LANCE_DRAGON: return "ドラ槍";
        case SKILL_LANCE_TEMPEST: return "テンペスト";
        case SKILL_LANCE_TYPHOON: return "タイフン";
        case SKILL_ARROW_SPLIT: return "スプリト";
        case SKILL_ARROW_BARRAGE: return "バラージ";
        case SKILL_ARROW_METEOR: return "メテオ矢";
        case SKILL_ARROW_SNIPE: return "スナイプ";
        case SKILL_ARROW_AURORA: return "オーロラ";
        case SKILL_FIST_COMBO: return "コンボ";
        case SKILL_FIST_SHOCK: return "ショック";
        case SKILL_FIST_RAGE: return "レイジ";
        case SKILL_FIST_HURRICANE: return "ハリケン";
        case SKILL_FIST_PHOENIX: return "フェニク";
        case SKILL_TOOL_LASER: return "レーザ";
        case SKILL_TOOL_QUAKE: return "シェイク";
        case SKILL_TOOL_PLASMA: return "プラズマ装";
        case SKILL_TOOL_REPAIR: return "リペア";
        case SKILL_TOOL_SHIELD: return "シールド";
        case SKILL_MAGIC_BLAZE: return "ブレイズ";
        case SKILL_MAGIC_INFERNO: return "インフェル";
        case SKILL_MAGIC_FIREWALL: return "ファイア壁";
        case SKILL_MAGIC_METEOR_FLARE: return "メテオフレア";
        case SKILL_MAGIC_FROST: return "フロスト";
        case SKILL_MAGIC_ABS_ZERO: return "ゼロ度";
        case SKILL_MAGIC_ICE_AGE: return "アイス世";
        case SKILL_MAGIC_SPARK: return "スパーク";
        case SKILL_MAGIC_BOLT: return "ボルト";
        case SKILL_MAGIC_JUDGMENT: return "ジャジ";
        case SKILL_MAGIC_SAINT: return "セイント";
        case SKILL_MAGIC_PRISM: return "プリズム";
        case SKILL_MAGIC_SHADOW: return "シャドウ";
        case SKILL_MAGIC_NIGHTMARE: return "ナイトメア";
        case SKILL_MAGIC_VOID_BREAK: return "ボイド";
        case SKILL_MAGIC_RECOVER: return "リカバ";
        case SKILL_MAGIC_REGEN: return "リジェン";
        case SKILL_MAGIC_GROUP_HEAL: return "グルプヒル";
        case SKILL_MAGIC_BOOST: return "ブースト";
        case SKILL_MAGIC_AEGIS: return "イージス";
        case SKILL_MAGIC_HASTE: return "ヘイスト";
        default: return "なし";
    }
}

static char runtime_skill_name_buf[24];

const char *battle_skill_runtime_name_buffered(UINT8 skill_id) BANKED {
    const char *src;
    UINT8 i;
    src = battle_skill_runtime_name(skill_id);
    i = 0u;
    while (i < 23u && src[i] != '\0') {
        runtime_skill_name_buf[i] = src[i];
        i++;
    }
    runtime_skill_name_buf[i] = '\0';
    return runtime_skill_name_buf;
}

void battle_skill_runtime_put_name(UINT8 x, UINT8 y, UINT8 skill_id) BANKED {
    jp_put_bkg_text(x, y, battle_skill_runtime_name(skill_id));
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
            jp_put_bkg_text(10u, (UINT8)(12u + row), battle_skill_runtime_name(skill_ids[idx]));
        }
    }
    jp_put_bkg_text(9u, 16u, "Aけってい Bもどる");
}

static void build_skill_list(UINT8 party_turn_slot, UINT8 magic_mode) {
    UINT8 i;
    UINT8 count;
    UINT8 sid;

    skill_count = 0u;
    list_magic_mode = magic_mode;
    for (i = 0u; i < PLAYER_SKILL_SLOT_COUNT; i++) skill_ids[i] = SKILL_NONE;

    count = party_get_learned_skill_count(party_turn_slot, magic_mode);
    for (i = 0u; i < count; i++) {
        sid = party_get_learned_skill_at(party_turn_slot, i, magic_mode);
        if (sid != SKILL_NONE) add_skill_choice(sid);
    }

    if (!magic_mode && skill_count == 0u) add_skill_choice(SKILL_POWER_STRIKE);
    if ( magic_mode && skill_count == 0u) add_skill_choice(SKILL_HEAL_SIMPLE);

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
