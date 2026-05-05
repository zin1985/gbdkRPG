#include <gb/gb.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "comdef.h"
#include "misakiUTF16.h"
#include "font_data_bank.h"
#define FTABLESIZE     font_data_table_size()

static void rotateToRight(byte *data);

static const uint8_t _hkremap[] = {
   0x02,0x0C,0x0D,0x01,0xFB,0xF2,0xA1,0xA3,0xA5,0xA7,0xA9,0xE3,0xE5,0xE7,0xC3,0xFD,
   0xA2,0xA4,0xA6,0xA8,0xAA,0xAB,0xAD,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,0xBB,0xBD,0xBF,
   0xC1,0xC4,0xC6,0xC8,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD2,0xD5,0xD8,0xDB,0xDE,0xDF,
   0xE0,0xE1,0xE2,0xE4,0xE6,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEF,0xF3,0x9B,0x9C
};

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

static uint8_t isHkana(uint16_t ucode) {
    return (ucode >= 0xFF61 && ucode <= 0xFF9F) ? 1u : 0u;
}

uint16_t hkana2kana(uint16_t utf16) {
    if (isHkana(utf16)) {
        return _hkremap[utf16 - 0xFF61] + 0x3000;
    }
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

    if (utf16 > 0x00FF || utf16 < 0x0021) {
        return utf16;
    }

    switch (utf16) {
        case 0x005C:
        case 0x00A2:
        case 0x00A3:
        case 0x00A7:
        case 0x00A8:
        case 0x00AC:
        case 0x00B0:
        case 0x00B1:
        case 0x00B4:
        case 0x00B6:
        case 0x00D7:
        case 0x00F7: return utf16;
        case 0x00A5: return 0xFFE5;

        case 0x0021: return 0xFF01;
        case 0x0022: return 0x201D;
        case 0x0023: return 0xFF03;
        case 0x0024: return 0xFF04;
        case 0x0025: return 0xFF05;
        case 0x0026: return 0xFF06;
        case 0x0027: return 0x2019;
        case 0x0028: return 0xFF08;
        case 0x0029: return 0xFF09;
        case 0x002A: return 0xFF0A;
        case 0x002B: return 0xFF0B;
        case 0x002C: return 0xFF0C;
        case 0x002D: return 0x2212;
        case 0x002E: return 0xFF0E;
        default:
            return utf16 - 0x002F + 0xFF0F;
    }
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
