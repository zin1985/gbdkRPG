#ifndef ITIL_QUIZ_BANK_H
#define ITIL_QUIZ_BANK_H

#include <gb/gb.h>

BANKREF_EXTERN(itil_quiz_data_bank)

UINT8 itil_quiz_count(void) BANKED;
void itil_quiz_draw(UINT8 idx, UINT8 cursor) BANKED;
UINT8 itil_quiz_is_correct(UINT8 idx, UINT8 cursor) BANKED;

#endif
