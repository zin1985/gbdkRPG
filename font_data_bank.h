#ifndef FONT_DATA_BANK_H
#define FONT_DATA_BANK_H

#include <gb/gb.h>
#include <stdint.h>
#include "comdef.h"

#define MISAKI_FONT_LEN 7u

UINT16 font_data_table_size(void) NONBANKED;
UINT16 font_data_table_at(UINT16 index) NONBANKED;
void font_data_read(UINT16 address, byte *rcvdata, byte n) NONBANKED;

#endif
