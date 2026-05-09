#pragma bank 13
#include "sunset_strings_adventure_field_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define SUNSET_STRINGS_FIELD_LEN 384u
#define SUNSET_STRINGS_FIELD_STEP_FRAMES 10u

/*
 * Original sunset strings adventure field BGM for Game Boy / GBDK.
 *
 * Mood:
 * - sad sunset field theme with a high, sustained, string-like melody
 * - more adventurous and majestic than the previous version
 * - still fragile, suitable for a ruined world overworld
 *
 * Note:
 * - The Game Boy cannot play real strings.
 * - This player simulates string-like sustain by NOT retriggering CH1/CH2
 *   when the same note continues across steps.
 *
 * Channels:
 * - CH1: high sustained lead
 * - CH2: sustained harmony/counter melody
 * - CH3: broad bass drone
 * - CH4: soft wind / distant pulse
 */

static const uint16_t pulse_freq_table[] = {
       0u,  458u,  547u,  711u,  856u,  986u, 1046u, 1155u, 1253u, 1297u,
    1379u, 1452u, 1517u, 1547u, 1602u, 1650u, 1673u, 1714u, 1750u, 1783u,
    1798u, 1825u, 1849u, 1860u, 1881u, 1899u, 1915u, 1923u, 1936u, 1949u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1253u, 1297u, 1379u, 1452u, 1517u, 1547u, 1602u, 1650u, 1673u,
    1714u, 1750u, 1783u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u, 1915u,
    1923u, 1936u, 1949u, 1954u, 1964u, 1974u, 1982u, 1985u, 1992u, 1998u,
};

static const uint8_t strings_lead_seq[SUNSET_STRINGS_FIELD_LEN] = {
    18, 18, 20, 20, 21, 21, 22, 22, 23, 22, 21,  0, 20,  0, 18,  0, 17, 17, 18, 18, 20, 20, 21, 21, 22, 21, 20,  0, 18,  0, 17,  0,
    16, 16, 18, 18, 20, 20, 21, 21, 22,  0, 23, 22, 21,  0, 20,  0, 18,  0, 20, 21, 22,  0, 21, 20, 18,  0, 17,  0, 18,  0,  0,  0,
    18, 20, 21,  0, 22,  0, 23, 22, 21, 20, 18,  0, 20,  0, 21,  0, 22,  0, 23, 24, 25,  0, 24, 23, 22, 21, 20,  0, 18,  0, 20,  0,
    21,  0, 22, 23, 24,  0, 23, 22, 21, 20, 18,  0, 17,  0, 18, 20, 21, 22, 23,  0, 22, 21, 20,  0, 18,  0, 17,  0, 18,  0,  0,  0,
    27, 27, 27, 27, 26, 26, 25, 25, 24, 24, 24, 24, 22, 22, 23, 23, 25, 25, 25, 25, 24, 24, 23, 23, 22, 22, 22, 22, 20, 20, 21, 21,
    23, 23, 23, 23, 24, 24, 25, 25, 27, 27, 27, 27, 26, 26, 25, 25, 24, 24, 24, 23, 22, 22, 21, 21, 20, 20, 20, 20, 18, 18, 18, 18,
    25, 25, 25, 25, 27, 27, 28, 28, 29, 29, 29, 29, 28, 28, 27, 27, 25, 25, 25, 24, 23, 23, 22, 22, 21, 21, 21, 21, 22, 22, 23, 23,
    24, 24, 24, 24, 25, 25, 27, 27, 25, 25, 25, 25, 24, 24, 23, 23, 22, 22, 21, 21, 20, 20, 20, 20, 18, 18, 17, 17, 18, 18, 18, 18,
    18, 18, 20, 20, 21, 21, 22, 22, 23, 22, 21,  0, 20,  0, 18,  0, 17, 17, 18, 18, 20, 20, 21, 21, 22, 21, 20,  0, 18,  0, 17,  0,
    16, 16, 18, 18, 20, 20, 21, 21, 22,  0, 23, 22, 21,  0, 20,  0, 18,  0, 20, 21, 22,  0, 21, 20, 18,  0, 17,  0, 18,  0,  0,  0,
    27, 27, 27, 27, 25, 25, 24, 24, 23, 23, 23, 23, 22, 22, 21, 21, 22, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 25, 27, 27, 25, 25,
    24, 24, 24, 24, 23, 23, 22, 22, 21, 21, 21, 21, 20, 20, 18, 18, 16, 16, 17, 17, 18, 18, 18, 18, 20, 20, 18, 18, 18, 18, 18, 18,
};

static const uint8_t strings_harmony_seq[SUNSET_STRINGS_FIELD_LEN] = {
    15, 15, 18, 18, 20, 20, 18, 18, 21, 20, 18,  0, 15,  0, 13,  0, 14, 14, 15, 15, 18, 18, 20, 20, 21, 20, 18,  0, 15,  0, 14,  0,
    13, 13, 16, 16, 18, 18, 20, 20, 21,  0, 20, 18, 16,  0, 15,  0, 11,  0, 15, 16, 17,  0, 16, 15, 13,  0, 14,  0, 15,  0,  0,  0,
    15, 18, 20,  0, 20,  0, 21, 20, 18, 15, 16,  0, 18,  0, 20,  0, 20,  0, 21, 22, 23,  0, 22, 21, 20, 18, 15,  0, 13,  0, 15,  0,
    16,  0, 17, 18, 20,  0, 18, 17, 16, 15, 13,  0, 14,  0, 15, 18, 16, 17, 18,  0, 17, 16, 15,  0, 13,  0, 14,  0, 15,  0,  0,  0,
    25, 25, 25, 25, 24, 24, 23, 23, 22, 22, 22, 22, 20, 20, 21, 21, 23, 23, 23, 23, 22, 22, 21, 21, 20, 20, 20, 20, 18, 18, 19, 19,
    21, 21, 21, 21, 22, 22, 23, 23, 25, 25, 25, 25, 24, 24, 23, 23, 22, 22, 22, 21, 20, 20, 18, 18, 17, 17, 17, 17, 15, 15, 15, 15,
    22, 22, 22, 22, 25, 25, 26, 26, 27, 27, 27, 27, 26, 26, 25, 25, 23, 23, 23, 22, 21, 21, 20, 20, 18, 18, 18, 18, 20, 20, 21, 21,
    22, 22, 22, 22, 23, 23, 25, 25, 23, 23, 23, 23, 22, 22, 21, 21, 20, 20, 18, 18, 17, 17, 17, 17, 15, 15, 14, 14, 15, 15, 15, 15,
    15, 15, 18, 18, 20, 20, 18, 18, 21, 20, 18,  0, 15,  0, 13,  0, 14, 14, 15, 15, 18, 18, 20, 20, 21, 20, 18,  0, 15,  0, 14,  0,
    13, 13, 16, 16, 18, 18, 20, 20, 21,  0, 20, 18, 16,  0, 15,  0, 11,  0, 15, 16, 17,  0, 16, 15, 13,  0, 14,  0, 15,  0,  0,  0,
    25, 25, 25, 25, 23, 23, 22, 22, 21, 21, 21, 21, 20, 20, 18, 18, 20, 20, 20, 20, 21, 21, 22, 22, 23, 23, 23, 23, 25, 25, 23, 23,
    22, 22, 22, 22, 21, 21, 20, 20, 18, 18, 18, 18, 17, 17, 15, 15, 14, 14, 15, 15, 15, 15, 15, 15, 18, 18, 15, 15, 15, 15, 15, 15,
};

static const uint8_t strings_bass_seq[SUNSET_STRINGS_FIELD_LEN] = {
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,
     2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  6,  0,  6,  0, 10,  0,  6,  0,  6,  0, 10,  0,  6,  0,  6,  0,
     2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  1,  0,  1,  0,  5,  0,  1,  0,  1,  0,  5,  0,  1,  0,  1,  0,
     2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  1,  0,  1,  0,  5,  0,  1,  0,  1,  0,  5,  0,  1,  0,  1,  0,
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,
     3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,
     2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,
     2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  6,  0,  2,  0,  2,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,  7,  0,  3,  0,  3,  0,
     4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,  8,  0,  4,  0,  4,  0,
};


static uint8_t strings_pos = 0u;
static uint8_t strings_tick = 0u;
static uint8_t strings_playing = 0u;
static uint8_t strings_ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;
static uint8_t last_harmony_note = 255u;

static void load_strings_wave(void) {
    static const uint8_t wave_data[16] = {
        0x8Au,0xBDu,0xEEu,0xFFu,0xECu,0xA8u,0x64u,0x31u,
        0x13u,0x46u,0x8Au,0xCEu,0xFEu,0xDCu,0x98u,0x64u
    };

    uint8_t i;
    NR30_REG = 0x00u;
    for (i = 0u; i != 16u; ++i) {
        *((uint8_t *)0xFF30u + i) = wave_data[i];
    }
    NR30_REG = 0x80u;
    strings_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u;
    NR12_REG = 0x87u; /* slow decay for sustained string-like tone */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_harmony_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0x40u;
    NR22_REG = 0x67u; /* soft slow decay */
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!strings_ch3_loaded) {
        load_strings_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_soft_noise(uint8_t kind) {
    NR41_REG = 0x20u;

    if (kind == 2u) {
        NR42_REG = 0x41u;
        NR43_REG = 0x65u;
    } else if (kind == 1u) {
        NR42_REG = 0x32u;
        NR43_REG = 0x46u;
    } else {
        NR42_REG = 0x21u;
        NR43_REG = 0x67u;
    }

    NR44_REG = 0x80u;
}

static void strings_play_step(uint8_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t lead_note = strings_lead_seq[pos];
    uint8_t harmony_note = strings_harmony_seq[pos];

    /*
     * String-like sustain:
     * If the note is the same as the previous step, do not retrigger.
     */
    if (lead_note != last_lead_note) {
        trigger_lead_ch1(pulse_freq_table[lead_note]);
        last_lead_note = lead_note;
    }

    if (harmony_note != last_harmony_note) {
        trigger_harmony_ch2(pulse_freq_table[harmony_note]);
        last_harmony_note = harmony_note;
    }

    set_bass_ch3(wave_freq_table[strings_bass_seq[pos]], ((beat & 7u) == 0u));

    if (beat == 0u) {
        trigger_soft_noise(2u);
    } else if (beat == 8u) {
        trigger_soft_noise(1u);
    } else if ((beat == 6u) || (beat == 14u)) {
        trigger_soft_noise(0u);
    }
}

void sunset_strings_adventure_field_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    strings_ch3_loaded = 0u;
    load_strings_wave();

    strings_pos = 0u;
    strings_tick = 0u;
    strings_playing = 1u;
    last_lead_note = 255u;
    last_harmony_note = 255u;

    strings_play_step(0u);
}

void sunset_strings_adventure_field_bgm_update(void) BANKED {
    if (!strings_playing) return;

    ++strings_tick;
    if (strings_tick >= SUNSET_STRINGS_FIELD_STEP_FRAMES) {
        strings_tick = 0u;
        ++strings_pos;

        if (strings_pos >= SUNSET_STRINGS_FIELD_LEN) {
            strings_pos = 0u;
            last_lead_note = 255u;
            last_harmony_note = 255u;
        }

        strings_play_step(strings_pos);
    }
}

void sunset_strings_adventure_field_bgm_stop(void) BANKED {
    strings_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
