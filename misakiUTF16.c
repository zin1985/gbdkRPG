#include <gb/gb.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "comdef.h"
#include "misakiUTF16.h"
#include "font_data_bank.h"
#define FTABLESIZE     font_data_table_size()

static void rotateToRight(byte *data);

/* rpg185: half-width kana table removed. Half-width kana is not used in this project. */


static byte Sequential_read(uint32_t address, byte* rcvdata, byte n) {
    font_data_read((UINT16)address, rcvdata, n);
    return n;
}

int findcode(uint16_t ucode) {
    int t_p = 0;
    int e_p = FTABLESIZE - 1;
    int pos;
    uint16_t d = 0;
    int flg_stop = 0;

    while (1) {
        pos = t_p + ((e_p - t_p + 1) >> 1);
        d = font_data_table_at((UINT16)pos);
        if (d == ucode) {
            flg_stop = 1;
            break;
        } else if (ucode > d) {
            t_p = pos + 1;
            if (t_p > e_p) break;
        } else {
            e_p = pos - 1;
            if (e_p < t_p) break;
        }
    }

    if (!flg_stop) return -1;
    return pos;
}

/* rpg185: half-width kana support removed to keep the font table focused on RPG Japanese. */
uint16_t hkana2kana(uint16_t utf16) {
    return utf16;
}

bool getFontDataByUTF16(byte*fontdata, uint16_t utf16) {
    int16_t code;
    bool rc = true;

    code = findcode(utf16);
    if (code < 0) {
        code = findcode(FONT_TOFU);
        rc = false;
    }

    if (FONT_LEN == Sequential_read((uint32_t)code * 7u, fontdata, (byte)FONT_LEN)) {
        fontdata[7] = 0;
    } else {
        rc = false;
    }
    return rc;
}

uint16_t utf16_HantoZen(uint16_t utf16) {
    utf16 = hkana2kana(utf16);

    /*
     * rpg185:
     * Keep ASCII as ASCII.  The project no longer keeps fullwidth alnum glyphs
     * in the Misaki table, and half-width alnum is enough for numbers, IDs,
     * money and debug text.  Japanese punctuation typed as UTF-8 fullwidth
     * characters still renders through the font table.
     */
    return utf16;
}

byte charUFT8toUTF16(uint16_t *pUTF16, const char *pUTF8) {
    byte bytes[3];
    uint16_t unicode16;

    bytes[0] = (byte)*pUTF8++;
    if (bytes[0] < 0x80) {
        *pUTF16 = bytes[0];
        return 1;
    }

    bytes[1] = (byte)*pUTF8++;
    if (bytes[0] >= 0xC0 && bytes[0] < 0xE0) {
        unicode16 = (uint16_t)(0x1Fu & bytes[0]);
        *pUTF16 = (uint16_t)((unicode16 << 6) + (0x3Fu & bytes[1]));
        return 2;
    }

    bytes[2] = (byte)*pUTF8++;
    if (bytes[0] >= 0xE0 && bytes[0] < 0xF0) {
        unicode16 = (uint16_t)(0x0Fu & bytes[0]);
        unicode16 = (uint16_t)((unicode16 << 6) + (0x3Fu & bytes[1]));
        *pUTF16 = (uint16_t)((unicode16 << 6) + (0x3Fu & bytes[2]));
        return 3;
    }

    return 0;
}

int16_t Utf8ToUtf16(uint16_t*pUTF16, const char *pUTF8) {
    int16_t len = 0;
    byte n;

    while (*pUTF8) {
        n = charUFT8toUTF16(pUTF16, pUTF8);
        if (n == 0) return -1;
        pUTF8 += n;
        len++;
        pUTF16++;
    }
    return len;
}

const char*getFontData(byte*fontdata, const char *pUTF8, bool h2z) {
    uint16_t utf16;
    byte n;

    if (pUTF8 == NULL || *pUTF8 == '\0') return NULL;

    n = charUFT8toUTF16(&utf16, pUTF8);
    if (n == 0) return NULL;

    if (h2z) {
        utf16 = utf16_HantoZen(utf16);
    }

    getFontDataByUTF16(fontdata, utf16);
    rotateToRight(fontdata);
    return pUTF8 + n;
}

const uint8_t*getFontTableAddress(void) {
    /*
     * rpg066:
     * fdata now lives in font_data_bank.c (ROM bank 6), so this function
     * cannot return a stable direct pointer from non-banked code.
     * It is kept only for compatibility and should not be used by gameplay.
     */
    return (const uint8_t*)0;
}

static void rotateToRight(byte *data) {
    uint8_t i, j;
    byte buf[8];

    for (i = 0; i < 8; i++) buf[i] = 0;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            buf[j] |= ((data[i] >> (7 - j)) & 1u) << i;
        }
    }

    for (i = 0; i < 8; i++) {
        data[i] = buf[i];
    }
}
