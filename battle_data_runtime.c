#include <gb/gb.h>
#include "battle_data.h"

BANKREF_EXTERN(battle_data_bank)

extern UINT8 battle_data_encounter_count_bank(void);
extern void battle_data_load_encounter_bank(UINT8 encounter_id, BattleEnemyData *out, UINT8 *count);
extern void battle_data_load_boss_bank(BattleEnemyData *out, UINT8 *count);

void battle_data_load_random_area(UINT8 seed, UINT8 area, BattleEnemyData *out, UINT8 *count) NONBANKED {
    UINT8 save_bank;
    UINT8 encounter_count;
    UINT8 encounter_id;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(battle_data_bank));

    encounter_count = battle_data_encounter_count_bank();
    if (encounter_count == 0u) {
        encounter_id = 0u;
    } else if (area >= 7u && area <= 10u) {
        encounter_id = (UINT8)(8u + (seed % 2u));
    } else if (area == 6u) {
        encounter_id = (UINT8)(2u + (seed % 2u));
    } else {
        encounter_id = (UINT8)(seed % 8u);
    }

    battle_data_load_encounter_bank(encounter_id, out, count);

    SWITCH_ROM(save_bank);
}

void battle_data_load_random(UINT8 seed, BattleEnemyData *out, UINT8 *count) NONBANKED {
    battle_data_load_random_area(seed, 0u, out, count);
}

void battle_data_load_boss(BattleEnemyData *out, UINT8 *count) NONBANKED {
    UINT8 save_bank;
    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(battle_data_bank));
    battle_data_load_boss_bank(out, count);
    SWITCH_ROM(save_bank);
}
