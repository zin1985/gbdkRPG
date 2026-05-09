#pragma bank 5

#include <gb/gb.h>
#include "audio.h"
#include "jpfont.h"
#include "party_runtime.h"
#include "battle_skill_runtime.h"

BANKREF(battle_skill_runtime)

#define SKILL_NONE         0u
#define SKILL_POWER_STRIKE 1u
#define SKILL_HEAL_SIMPLE  2u
#define SKILL_FIRE         3u
#define SKILL_GUARD_BREAK  4u

#define PLAYER_SKILL_SLOT_COUNT 4u

#define SKILL_KIND_DAMAGE 0u
#define SKILL_KIND_HEAL   1u

/* main.c still owns the player's learned skill slots for now. */
UINT8 player_get_skill_slot(UINT8 slot);

static UINT8 skill_cursor;
static UINT8 skill_count;
static UINT8 skill_ids[PLAYER_SKILL_SLOT_COUNT];
static UINT8 selected_skill;
static UINT8 ally_cursor;

static const char *runtime_skill_name(UINT8 skill_id) {
    if (skill_id == SKILL_POWER_STRIKE) return "強げき";
    if (skill_id == SKILL_HEAL_SIMPLE) return "かいふく";
    if (skill_id == SKILL_FIRE) return "ひの術";
    if (skill_id == SKILL_GUARD_BREAK) return "くずし";
    return "なし";
}

static UINT8 runtime_skill_kind(UINT8 skill_id) {
    if (skill_id == SKILL_HEAL_SIMPLE) return SKILL_KIND_HEAL;
    return SKILL_KIND_DAMAGE;
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

static void draw_skill_window(void) {
    UINT8 i;
    jp_draw_bkg_frame(9u, 9u, 11u, 5u);
    jp_bkg_clear_area(10u, 10u, 9u, 3u);
    for (i = 0u; i < skill_count && i < 3u; i++) {
        jp_put_bkg_text(10u, (UINT8)(10u + i), (i == skill_cursor) ? ">" : " ");
        jp_put_bkg_text(11u, (UINT8)(10u + i), runtime_skill_name(skill_ids[i]));
    }
}

static void build_skill_list(UINT8 party_turn_slot) {
    UINT8 member_id;
    UINT8 i;

    skill_count = 0u;
    for (i = 0u; i < PLAYER_SKILL_SLOT_COUNT; i++) skill_ids[i] = SKILL_NONE;

    member_id = party_get_active_member_id(party_turn_slot);
    if (member_id == PARTY_MEMBER_PRIEST) {
        add_skill_choice(SKILL_HEAL_SIMPLE);
        add_skill_choice(SKILL_GUARD_BREAK);
    } else if (member_id == PARTY_MEMBER_MAGE) {
        add_skill_choice(SKILL_FIRE);
        add_skill_choice(SKILL_HEAL_SIMPLE);
    } else {
        for (i = 0u; i < PLAYER_SKILL_SLOT_COUNT; i++) {
            add_skill_choice(player_get_skill_slot(i));
        }
    }
    if (skill_count == 0u) add_skill_choice(SKILL_POWER_STRIKE);
    skill_cursor = 0u;
}

void battle_skill_runtime_start(UINT8 party_turn_slot) BANKED {
    build_skill_list(party_turn_slot);
    draw_skill_window();
}

UINT8 battle_skill_runtime_update(UINT8 *skill_id) BANKED {
    UINT8 keys;
    keys = joypad();
    if (keys & J_UP) {
        if (skill_cursor == 0u) skill_cursor = (UINT8)(skill_count - 1u);
        else skill_cursor--;
        draw_skill_window();
        audio_waitpadup();
    } else if (keys & J_DOWN) {
        skill_cursor++;
        if (skill_cursor >= skill_count) skill_cursor = 0u;
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
    jp_draw_bkg_frame(9u, 9u, 11u, 5u);
    jp_bkg_clear_area(10u, 10u, 9u, 3u);
    for (i = 0u; i < PARTY_ACTIVE_COUNT; i++) {
        jp_put_bkg_text(10u, (UINT8)(10u + i), (i == ally_cursor) ? ">" : " ");
        jp_put_bkg_text(11u, (UINT8)(10u + i), party_get_active_name(i));
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
