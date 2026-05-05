#include <gb/gb.h>
#include <stdint.h>
#include "comdef.h"
#include "font_data_bank.h"

BANKREF_EXTERN(font_data_bank)

extern UINT16 font_data_table_size_bank(void);
extern UINT16 font_data_table_at_bank(UINT16 index);
extern void font_data_read_bank(UINT16 address, byte *rcvdata, byte n);

UINT16 font_data_table_size(void) NONBANKED {
    UINT8 save_bank;
    UINT16 value;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(font_data_bank));
    value = font_data_table_size_bank();
    SWITCH_ROM(save_bank);

    return value;
}

UINT16 font_data_table_at(UINT16 index) NONBANKED {
    UINT8 save_bank;
    UINT16 value;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(font_data_bank));
    value = font_data_table_at_bank(index);
    SWITCH_ROM(save_bank);

    return value;
}

void font_data_read(UINT16 address, byte *rcvdata, byte n) NONBANKED {
    UINT8 save_bank;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(BANK(font_data_bank));
    font_data_read_bank(address, rcvdata, n);
    SWITCH_ROM(save_bank);
}
