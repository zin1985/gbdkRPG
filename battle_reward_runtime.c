#pragma bank 21

#include <gb/gb.h>
#include "bank_ids.h"
#include "battle_reward_runtime.h"
#include "inventory.h"

BANKREF(battle_reward_runtime)


static char reward_msg[32];

static const char *reward_format_gold(UINT16 value) BANKED {
    char tmp[6];
    UINT8 ti;
    UINT8 di;
    reward_msg[0] = (char)0xE3u; reward_msg[1] = (char)0x81u; reward_msg[2] = (char)0x8Au; /* お */
    reward_msg[3] = (char)0xE3u; reward_msg[4] = (char)0x81u; reward_msg[5] = (char)0x8Bu; /* か */
    reward_msg[6] = (char)0xE3u; reward_msg[7] = (char)0x81u; reward_msg[8] = (char)0xADu; /* ね */
    reward_msg[9] = '\n';
    ti = 0u;
    do {
        tmp[ti++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value != 0u && ti < sizeof(tmp));
    di = 10u;
    while (ti > 0u && di < 28u) reward_msg[di++] = tmp[--ti];
    reward_msg[di++] = 'G';
    reward_msg[di++] = 'E';
    reward_msg[di++] = 'T';
    reward_msg[di] = '\0';
    return reward_msg;
}

/* Owned by main.c battle UI.  Kept non-banked because it touches battle state. */
void battle_show_message(const char *text);
void battle_show_damage_message(const char *prefix, UINT16 value);

UINT8 battle_reward_runtime_apply(UINT8 enemy_rank, UINT8 rand_a, UINT8 rand_b, UINT8 cave_boss_pending) BANKED {
    UINT16 gold_reward;
    UINT8 drop_item;

    gold_reward = (UINT16)(20u + (UINT16)enemy_rank * 18u);
    inventory_add_money(gold_reward);
    battle_show_message(reward_format_gold(gold_reward));

    drop_item = ITEM_NONE;
    if ((rand_a & 0x0Fu) == 0u) {
        drop_item = ITEM_POWER_SEED;
    } else if ((rand_b & 0x03u) == 0u) {
        drop_item = ITEM_HIGH_POTION;
    }

    if (drop_item != ITEM_NONE) {
        inventory_add(drop_item, 1u);
        battle_show_message("アイテムを てにいれた");
        battle_show_message(inventory_get_item_name(drop_item));
    }

    if (cave_boss_pending) {
        inventory_add_money(300u);
        battle_show_message(reward_format_gold(300u));
        inventory_add(ITEM_DRAGON_SWORD, 1u);
        inventory_add(ITEM_DRAGON_ARMOR, 1u);
        battle_show_message("ボスを たおした!");
        battle_show_message("竜のけんを てにいれた");
        battle_show_message("竜よろいを てにいれた");
        return 1u;
    }

    return 0u;
}
