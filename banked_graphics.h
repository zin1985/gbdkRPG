#ifndef BANKED_GRAPHICS_H
#define BANKED_GRAPHICS_H

#include <gb/gb.h>

void set_banked_bkg_data(UINT8 first_tile, UINT8 nb_tiles, const unsigned char *data, UINT8 bank) NONBANKED;
void set_banked_sprite_data(UINT8 first_tile, UINT8 nb_tiles, const unsigned char *data, UINT8 bank) NONBANKED;

#endif
