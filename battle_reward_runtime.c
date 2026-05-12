#pragma bank 21

#include <gb/gb.h>
#include "bank_ids.h"
#include "battle_reward_runtime.h"
#include "inventory.h"

BANKREF(battle_reward_runtime)

/* Owned by main.c battle UI.  Kept non-banked because it touches battle state. */
void battle_show_message(const char *text);
void battle_show_damage_message(const char *prefix, UINT16 value);

UINT8 battle_reward_runtime_apply(UINT8 enemy_rank, UINT8 rand_a, UINT8 rand_b, UINT8 cave_boss_pending) BANKED {
    UINT16 gold_reward;
    UINT8 drop_item;

    gold_reward = (UINT16)(20u + (UINT16)enemy_rank * 18u);
    inventory_add_money(gold_reward);
    battle_show_damage_message("おかね", gold_reward);

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
        inventory_add(ITEM_DRAGON_SWORD, 1u);
        inventory_add(ITEM_DRAGON_ARMOR, 1u);
        battle_show_message("ボスを たおした!");
        battle_show_message("竜のけんを てにいれた");
        battle_show_message("竜よろいを てにいれた");
        return 1u;
    }

    return 0u;
}
