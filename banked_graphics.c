#include <gb/gb.h>
#include "banked_graphics.h"

void set_banked_bkg_data(UINT8 first_tile, UINT8 nb_tiles, const unsigned char *data, UINT8 bank) NONBANKED {
    UINT8 save_bank;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(bank);
    set_bkg_data(first_tile, nb_tiles, data);
    SWITCH_ROM(save_bank);
}

void set_banked_sprite_data(UINT8 first_tile, UINT8 nb_tiles, const unsigned char *data, UINT8 bank) NONBANKED {
    UINT8 save_bank;

    save_bank = CURRENT_BANK;
    SWITCH_ROM(bank);
    set_sprite_data(first_tile, nb_tiles, data);
    SWITCH_ROM(save_bank);
}
