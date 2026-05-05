#pragma bank 6

#include <gb/gb.h>
#include <stdint.h>
#include "comdef.h"
#include "misakiUTF16FontData.h"

BANKREF(font_data_bank)

UINT16 font_data_table_size_bank(void) {
    return (UINT16)(sizeof(ftable) / sizeof(ftable[0]));
}

UINT16 font_data_table_at_bank(UINT16 index) {
    return ftable[index];
}

void font_data_read_bank(UINT16 address, byte *rcvdata, byte n) {
    byte i;
    for (i = 0u; i < n; i++) {
        rcvdata[i] = fdata[address + i];
    }
}
