#include <gb/gb.h>
#include "battle_data.h"

BANKREF_EXTERN(battle_data_bank)

extern void battle_data_load_encounter_bank(UINT8 encounter_id, BattleEnemyData *out, UINT8 *count);

void battle_data_load_random_area(UINT8 seed, UINT8 area, BattleEnemyData *out, UINT8 *count) NONBANKED {
    UINT8 save_bank;
    UINT8 encounter_id;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(battle_data_bank));

    if (area == 12u) { /* special: mimic chest */
        encounter_id = 13u;
    } else if (area == 13u) { /* special: cave boss */
        encounter_id = 12u;
    } else if (area == 14u) { /* special: plant pair debug chest */
        encounter_id = 8u;
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

