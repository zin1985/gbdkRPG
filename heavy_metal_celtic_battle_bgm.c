#pragma bank 15

#include "heavy_metal_celtic_battle_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define METAL_CELTIC_BATTLE_LEN 384u
#define METAL_CELTIC_BATTLE_STEP_FRAMES 6u
#define METAL_DROP_POS 64u

/*
 * Original heavy metal Celtic battle BGM for Game Boy / GBDK.
 *
 * Structure:
 * - Step 0-63: first phrase, restrained and ominous
 * - Step 64+: metal drop with low bass chugs and aggressive CH4 drums
 *
 * Channels:
 * - CH1: main heroic lead
 * - CH2: counter/rhythm guitar style pulse
 * - CH3: low drone / chugging bass
 * - CH4: battle drums
 */

static const uint16_t pulse_freq_table[] = {
       0u,   44u,  263u,  363u,  458u,  547u,  711u,  856u,  923u,  986u,
    1046u, 1155u, 1205u, 1253u, 1297u, 1340u, 1379u, 1452u, 1486u, 1517u,
    1547u, 1602u, 1627u, 1650u, 1673u, 1694u, 1714u, 1750u, 1767u, 1783u,
    1798u, 1825u, 1837u, 1849u, 1860u, 1871u, 1881u, 1899u, 1907u, 1915u,
    1923u, 1936u, 1943u, 1949u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1046u, 1155u, 1205u, 1253u, 1297u, 1379u, 1452u, 1486u, 1517u,
    1547u, 1602u, 1627u, 1650u, 1673u, 1694u, 1714u, 1750u, 1767u, 1783u,
    1798u, 1825u, 1837u, 1849u, 1860u, 1871u, 1881u, 1899u, 1907u, 1915u,
    1923u, 1936u, 1943u, 1949u, 1954u, 1959u, 1964u, 1974u, 1978u, 1982u,
    1985u, 1992u, 1995u, 1998u,
};

static const uint8_t metal_lead_seq[METAL_CELTIC_BATTLE_LEN] = {
    31,  0, 31, 34, 37,  0, 36, 34, 33,  0, 34, 36, 37,  0, 41,  0, 40,  0, 37, 36, 34,  0, 31, 34, 33,  0, 30, 31, 34,  0, 37,  0,
    38,  0, 37, 36, 37,  0, 41, 40, 37,  0, 36, 34, 33,  0, 31,  0, 34, 36, 37,  0, 40, 37, 36,  0, 34, 33, 31,  0, 31,  0, 27,  0,
    31, 34, 37, 41, 40, 37, 36, 34, 33, 34, 36, 37, 41, 40, 37, 36, 34, 31, 34, 37, 36, 33, 36, 40, 37, 36, 34, 33, 31, 27, 31,  0,
    36, 37, 38, 37, 36, 34, 33, 31, 30, 31, 34, 36, 37, 40, 41, 40, 37, 36, 34, 33, 31, 34, 37, 41, 40, 37, 36, 34, 33, 30, 31,  0,
    34, 36, 37, 38, 37, 36, 34, 33, 34, 37, 40, 41, 42, 41, 40, 37, 36, 37, 38, 37, 36, 34, 32, 31, 30, 31, 32, 34, 36, 37, 40, 37,
    31, 34, 37, 41, 42, 41, 40, 37, 38, 37, 36, 34, 33, 34, 36, 37, 40, 37, 36, 34, 33, 30, 31, 34, 37, 34, 33, 31, 31,  0, 27,  0,
    31, 34, 37, 41, 40, 37, 36, 34, 33, 34, 36, 37, 41, 40, 37, 36, 34, 31, 34, 37, 36, 33, 36, 40, 37, 36, 34, 33, 31, 27, 31,  0,
    38, 37, 36, 34, 37, 36, 34, 33, 31, 34, 37, 41, 40, 37, 36, 34, 33, 34, 36, 37, 36, 34, 33, 30, 31, 34, 37, 41, 41,  0, 37,  0,
    37, 40, 41, 43, 41, 40, 37, 36, 37, 40, 41, 40, 37, 36, 34, 33, 36, 37, 38, 40, 41, 40, 37, 36, 34, 36, 37, 40, 37, 36, 34, 31,
    33, 34, 36, 37, 40, 41, 42, 41, 40, 37, 36, 34, 33, 30, 31, 34, 41, 40, 37, 36, 34, 33, 31, 30, 31, 34, 37, 41, 40, 37, 36, 34,
    31, 34, 37, 41, 40, 37, 36, 34, 33, 34, 36, 37, 41, 40, 37, 36, 38, 37, 36, 34, 33, 34, 36, 37, 40, 37, 36, 34, 33, 30, 31, 34,
    37, 36, 34, 33, 31, 30, 27, 26, 24, 27, 31, 34, 37, 41, 40, 37, 36, 34, 33, 30, 31, 27, 31, 34, 31,  0, 27,  0, 31,  0, 31,  0,
};

static const uint8_t metal_guitar_seq[METAL_CELTIC_BATTLE_LEN] = {
    27,  0, 27, 30, 31,  0, 30, 27, 26,  0, 27, 30, 31,  0, 34,  0, 33,  0, 30, 27, 31,  0, 27, 31, 30,  0, 26, 27, 30,  0, 31,  0,
    34,  0, 33, 31, 33,  0, 37, 36, 33,  0, 31, 30, 27,  0, 24,  0, 27, 30, 31,  0, 33, 31, 30,  0, 27, 26, 24,  0, 24,  0, 23,  0,
    21, 21,  0, 27, 21, 21,  0, 31, 21, 21,  0, 27, 31, 27, 21, 27, 20, 20,  0, 26, 20, 20,  0, 30, 20, 20,  0, 26, 30, 26, 20, 26,
    18, 18,  0, 24, 18, 18,  0, 28, 18, 18,  0, 24, 28, 24, 18, 24, 17, 17,  0, 23, 17, 17,  0, 27, 17, 17,  0, 23, 27, 23, 17, 23,
    16, 16,  0, 21, 16, 16,  0, 26, 16, 16,  0, 21, 26, 21, 16, 21, 14, 14,  0, 20, 14, 14,  0, 24, 14, 14,  0, 20, 24, 20, 14, 20,
    21, 21,  0, 27, 21, 21,  0, 31, 21, 21,  0, 27, 31, 27, 21, 27, 20, 20,  0, 26, 20, 20,  0, 30, 20, 20,  0, 26, 30, 26, 20, 26,
    18, 18,  0, 24, 18, 18,  0, 28, 18, 18,  0, 24, 28, 24, 18, 24, 17, 17,  0, 23, 17, 17,  0, 27, 17, 17,  0, 23, 27, 23, 17, 23,
    16, 16,  0, 21, 16, 16,  0, 26, 16, 16,  0, 21, 26, 21, 16, 21, 14, 14,  0, 20, 14, 14,  0, 24, 14, 14,  0, 20, 24, 20, 14, 20,
    21, 21,  0, 27, 21, 21,  0, 31, 21, 21,  0, 27, 31, 27, 21, 27, 20, 20,  0, 26, 20, 20,  0, 30, 20, 20,  0, 26, 30, 26, 20, 26,
    18, 18,  0, 24, 18, 18,  0, 28, 18, 18,  0, 24, 28, 24, 18, 24, 17, 17,  0, 23, 17, 17,  0, 27, 17, 17,  0, 23, 27, 23, 17, 23,
    16, 16,  0, 21, 16, 16,  0, 26, 16, 16,  0, 21, 26, 21, 16, 21, 14, 14,  0, 20, 14, 14,  0, 24, 14, 14,  0, 20, 24, 20, 14, 20,
    21, 21,  0, 27, 21, 21,  0, 31, 21, 21,  0, 27, 31, 27, 21, 27, 20, 20,  0, 26, 20, 20,  0, 30, 20, 20,  0, 26, 30, 26, 20, 26,
};

static const uint8_t metal_bass_seq[METAL_CELTIC_BATTLE_LEN] = {
    11, 11, 17, 11, 11, 17, 11, 17, 11, 11, 17, 11, 17, 11, 17, 11, 10, 10, 16, 10, 10, 16, 10, 16, 10, 10, 16, 10, 16, 10, 16, 10,
     8,  8, 14,  8,  8, 14,  8, 14,  8,  8, 14,  8, 14,  8, 14,  8, 11, 11, 17, 11, 11, 17, 11, 17, 11, 11, 17, 11, 17, 11, 17, 11,
     2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,
     2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,
     1,  1,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  6,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  6,  1,  6,
     8,  8,  8,  5,  8,  8,  8,  5,  8,  8,  8,  5,  8,  5,  8,  5,  7,  7,  7,  4,  7,  7,  7,  4,  7,  7,  7,  4,  7,  4,  7,  4,
     2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,
     6,  6,  6, 11,  6,  6,  6, 11,  6,  6,  6, 11,  6, 11,  6, 11,  7,  7,  7,  4,  7,  7,  7,  4,  7,  7,  7,  4,  7,  4,  7,  4,
     2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,
     2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  2,  2,  7,  2,  7,  2,  7,
     1,  1,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  6,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  1,  1,  6,  1,  6,  1,  6,
     8,  8,  8,  5,  8,  8,  8,  5,  8,  8,  8,  5,  8,  5,  8,  5,  7,  7,  7,  4,  7,  7,  7,  4,  7,  7,  7,  4,  7,  4,  7,  4,
};


static uint8_t metal_pos = 0u;
static uint8_t metal_tick = 0u;
static uint8_t metal_playing = 0u;
static uint8_t metal_ch3_loaded = 0u;

static void load_metal_wave(void) {
    static const uint8_t wave_data[16] = {
        0x9Cu,0xDFu,0xFFu,0xECu,0xA7u,0x52u,0x10u,0x00u,
        0x00u,0x12u,0x57u,0xAEu,0xFFu,0xFDu,0xB8u,0x63u
    };

    uint8_t i;
    NR30_REG = 0x00u;
    for (i = 0u; i != 16u; ++i) {
        *((uint8_t *)0xFF30u + i) = wave_data[i];
    }
    NR30_REG = 0x80u;
    metal_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f, uint8_t metal_section) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u;
    NR12_REG = metal_section ? 0x83u : 0x84u;
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_guitar_ch2(uint16_t f, uint8_t metal_section) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = metal_section ? 0x40u : 0x80u;
    NR22_REG = metal_section ? 0x72u : 0x53u;
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!metal_ch3_loaded) {
        load_metal_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_drum_ch4(uint8_t kind, uint8_t metal_section) {
    NR41_REG = metal_section ? 0x08u : 0x18u;

    if (kind == 3u) {
        /* metal hat/tick */
        NR42_REG = 0x31u;
        NR43_REG = 0x26u;
    } else if (kind == 2u) {
        /* snare/crash */
        NR42_REG = metal_section ? 0x84u : 0x73u;
        NR43_REG = metal_section ? 0x34u : 0x35u;
    } else if (kind == 1u) {
        /* kick/thump */
        NR42_REG = metal_section ? 0x91u : 0x81u;
        NR43_REG = metal_section ? 0x56u : 0x57u;
    } else {
        /* small tick */
        NR42_REG = 0x31u;
        NR43_REG = 0x47u;
    }

    NR44_REG = 0x80u;
}

static void play_metal_step(uint8_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t metal_section = (pos >= METAL_DROP_POS);

    trigger_lead_ch1(pulse_freq_table[metal_lead_seq[pos]], metal_section);
    trigger_guitar_ch2(pulse_freq_table[metal_guitar_seq[pos]], metal_section);

    /* Intro bass is steadier. After drop, retrigger more often for chug. */
    set_bass_ch3(wave_freq_table[metal_bass_seq[pos]], metal_section ? 1u : ((beat & 3u) == 0u));

    if (metal_section) {
        if ((beat == 0u) || (beat == 8u)) {
            trigger_drum_ch4(1u, 1u);
        } else if ((beat == 4u) || (beat == 12u)) {
            trigger_drum_ch4(2u, 1u);
        } else if ((beat == 2u) || (beat == 6u) || (beat == 10u) || (beat == 14u)) {
            trigger_drum_ch4(3u, 1u);
        } else {
            trigger_drum_ch4(0u, 1u);
        }
    } else {
        if ((beat == 0u) || (beat == 8u)) {
            trigger_drum_ch4(1u, 0u);
        } else if ((beat == 4u) || (beat == 12u)) {
            trigger_drum_ch4(2u, 0u);
        } else if ((beat == 7u) || (beat == 15u)) {
            trigger_drum_ch4(0u, 0u);
        }
    }
}

void heavy_metal_celtic_battle_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    metal_ch3_loaded = 0u;
    load_metal_wave();

    metal_pos = 0u;
    metal_tick = 0u;
    metal_playing = 1u;

    play_metal_step(0u);
}

void heavy_metal_celtic_battle_bgm_update(void) BANKED {
    if (!metal_playing) return;

    ++metal_tick;
    if (metal_tick >= METAL_CELTIC_BATTLE_STEP_FRAMES) {
        metal_tick = 0u;
        ++metal_pos;

        if (metal_pos >= METAL_CELTIC_BATTLE_LEN) {
            metal_pos = 0u;
        }

        play_metal_step(metal_pos);
    }
}

void heavy_metal_celtic_battle_bgm_stop(void) BANKED {
    metal_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
