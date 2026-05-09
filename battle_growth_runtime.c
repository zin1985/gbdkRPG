#pragma bank 5

#include <gb/gb.h>
#include "battle_growth_runtime.h"
#include "messages.h"
#include "dialogue.h"

BANKREF(battle_growth_runtime_bank)

#define PLAYER_HP_MP_MAX 999u
#define PLAYER_STAT_MAX 255u
#define ENEMY_RANK_MAX 5u

#define GROWTH_NONE 0u
#define GROWTH_HP 1u
#define GROWTH_MP 2u
#define GROWTH_ATK 3u
#define GROWTH_DEF 4u
#define GROWTH_SKILL 5u
#define GROWTH_HEAL 6u
#define GROWTH_AGI 7u
#define GROWTH_STAT_COUNT 7u

typedef struct GrowthResult {
    UINT8 actor_id;
    UINT8 stat_type;
    UINT16 old_value;
    UINT16 new_value;
} GrowthResult;

extern UINT8 last_growth_type;
extern GrowthResult last_growth;
extern UINT8 battle_enemy_rank;

extern UINT16 player_max_hp_stat;
extern UINT16 player_max_mp_stat;
extern UINT8 player_attack_stat;
extern UINT8 player_defense_stat;
extern UINT8 player_skill_power_stat;
extern UINT8 player_heal_power_stat;
extern UINT8 player_agility_stat;

UINT8 random_u8(void);

static const UINT8 growth_chance_by_rank_bank[] = {0u, 40u, 45u, 50u, 55u, 60u};

void battle_growth_clear(void) BANKED {
    last_growth_type = GROWTH_NONE;
    last_growth.actor_id = 0u;
    last_growth.stat_type = GROWTH_NONE;
    last_growth.old_value = 0u;
    last_growth.new_value = 0u;
}

static UINT8 battle_growth_chance(UINT8 enemy_rank) {
    if (enemy_rank == 0u) {
        enemy_rank = 1u;
    }
    if (enemy_rank > ENEMY_RANK_MAX) {
        enemy_rank = ENEMY_RANK_MAX;
    }

    return growth_chance_by_rank_bank[enemy_rank];
}

static UINT8 battle_growth_should_happen(UINT8 enemy_rank) {
    return (UINT8)((random_u8() % 100u) < battle_growth_chance(enemy_rank));
}

static UINT8 battle_growth_apply_player_stat(UINT8 stat_type) {
    last_growth.actor_id = 0u;
    last_growth.stat_type = GROWTH_NONE;
    last_growth.old_value = 0u;
    last_growth.new_value = 0u;

    switch (stat_type) {
        case GROWTH_HP:
            if (player_max_hp_stat < PLAYER_HP_MP_MAX) {
                last_growth.old_value = player_max_hp_stat;
                player_max_hp_stat++;
                last_growth.new_value = player_max_hp_stat;
                last_growth.stat_type = GROWTH_HP;
                return 1u;
            }
            break;

        case GROWTH_MP:
            if (player_max_mp_stat < PLAYER_HP_MP_MAX) {
                last_growth.old_value = player_max_mp_stat;
                player_max_mp_stat++;
                last_growth.new_value = player_max_mp_stat;
                last_growth.stat_type = GROWTH_MP;
                return 1u;
            }
            break;

        case GROWTH_ATK:
            if (player_attack_stat < PLAYER_STAT_MAX) {
                last_growth.old_value = player_attack_stat;
                player_attack_stat++;
                last_growth.new_value = player_attack_stat;
                last_growth.stat_type = GROWTH_ATK;
                return 1u;
            }
            break;

        case GROWTH_DEF:
            if (player_defense_stat < PLAYER_STAT_MAX) {
                last_growth.old_value = player_defense_stat;
                player_defense_stat++;
                last_growth.new_value = player_defense_stat;
                last_growth.stat_type = GROWTH_DEF;
                return 1u;
            }
            break;

        case GROWTH_SKILL:
            if (player_skill_power_stat < PLAYER_STAT_MAX) {
                last_growth.old_value = player_skill_power_stat;
                player_skill_power_stat++;
                last_growth.new_value = player_skill_power_stat;
                last_growth.stat_type = GROWTH_SKILL;
                return 1u;
            }
            break;

        case GROWTH_HEAL:
            if (player_heal_power_stat < PLAYER_STAT_MAX) {
                last_growth.old_value = player_heal_power_stat;
                player_heal_power_stat++;
                last_growth.new_value = player_heal_power_stat;
                last_growth.stat_type = GROWTH_HEAL;
                return 1u;
            }
            break;

        case GROWTH_AGI:
            if (player_agility_stat < PLAYER_STAT_MAX) {
                last_growth.old_value = player_agility_stat;
                player_agility_stat++;
                last_growth.new_value = player_agility_stat;
                last_growth.stat_type = GROWTH_AGI;
                return 1u;
            }
            break;
    }

    return 0u;
}

void battle_growth_apply(void) BANKED {
    UINT8 stat_type;

    stat_type = last_growth_type;
    battle_growth_clear();

    if (!battle_growth_should_happen(battle_enemy_rank)) {
        return;
    }

    if (stat_type == GROWTH_SKILL) {
        stat_type = (random_u8() & 1u) ? GROWTH_SKILL : GROWTH_MP;
    } else if (stat_type == GROWTH_ATK) {
        stat_type = (random_u8() & 1u) ? GROWTH_ATK : GROWTH_AGI;
    } else if (stat_type == GROWTH_NONE) {
        stat_type = (UINT8)(GROWTH_HP + (random_u8() % GROWTH_STAT_COUNT));
    }
    battle_growth_apply_player_stat(stat_type);
}

void battle_growth_show_message(void) BANKED {
    if (last_growth.stat_type == GROWTH_HP) {
        message_show(MSG_GROWTH_HP);
    } else if (last_growth.stat_type == GROWTH_MP) {
        message_show(MSG_GROWTH_MP);
    } else if (last_growth.stat_type == GROWTH_ATK) {
        message_show(MSG_GROWTH_ATK);
    } else if (last_growth.stat_type == GROWTH_DEF) {
        message_show(MSG_GROWTH_DEF);
    } else if (last_growth.stat_type == GROWTH_SKILL) {
        message_show(MSG_GROWTH_SKILL);
    } else if (last_growth.stat_type == GROWTH_HEAL) {
        message_show(MSG_GROWTH_HEAL);
    } else if (last_growth.stat_type == GROWTH_AGI) {
        message_show(MSG_GROWTH_AGI);
    }
}
