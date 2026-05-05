#ifndef MISAKI_UTF16_H
#define MISAKI_UTF16_H

#include <stdint.h>
#include <stdbool.h>
#include "comdef.h"

/* rpg065 note: Misaki font table bytes live in ROM bank 6 through font_data_bank.c.
 * misakiUTF16.c code is non-banked again and accesses the tables through safe wrappers.
 * This avoids banked calls from inside the dialogue renderer.
 */
#define FONT_LEN       7
#define FONT_TOFU      0x25A1

int findcode(uint16_t ucode);
bool getFontDataByUTF16(byte*fontdata, uint16_t utf16);
uint16_t hkana2kana(uint16_t utf16);
uint16_t utf16_HantoZen(uint16_t utf16);
byte charUFT8toUTF16(uint16_t *pUTF16, const char *pUTF8);
int16_t Utf8ToUtf16(uint16_t*pUTF16, const char *pUTF8);
const char*getFontData(byte*fontdata, const char *pUTF8, bool h2z);
const uint8_t*getFontTableAddress(void);

#endif
