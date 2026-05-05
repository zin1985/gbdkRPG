#include <gb/gb.h>
#include "battle_data.h"

BANKREF_EXTERN(battle_data_bank)

extern UINT8 battle_data_encounter_count_bank(void);
extern void battle_data_load_encounter_bank(UINT8 encounter_id, BattleEnemyData *out, UINT8 *count);

void battle_data_load_random(UINT8 seed, BattleEnemyData *out, UINT8 *count) NONBANKED {
    UINT8 save_bank;
    UINT8 encounter_count;
    UINT8 encounter_id;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(battle_data_bank));

    encounter_count = battle_data_encounter_count_bank();
    if (encounter_count == 0u) {
        encounter_id = 0u;
    } else {
        encounter_id = (UINT8)(seed % encounter_count);
    }

    battle_data_load_encounter_bank(encounter_id, out, count);

    SWITCH_ROM(save_bank);
}
