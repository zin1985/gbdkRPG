#pragma bank 14

#include "sunset_ruins_field_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define SUNSET_FIELD_LEN 384u
#define SUNSET_FIELD_STEP_FRAMES 10u

/*
 * Original sunset ruins field BGM for Game Boy / GBDK.
 *
 * Mood:
 * - sad, sunset-colored field theme
 * - melodic, majestic, powerful, but fragile
 * - suitable for a ruined world, post-disaster overworld, or late-game field
 *
 * Channels:
 * - CH1: main lyrical lead
 * - CH2: soft counter melody / harmony
 * - CH3: broad bass drone
 * - CH4: soft wind / distant drum noise
 */

static const uint16_t pulse_freq_table[] = {
       0u,  458u,  547u,  711u,  856u,  923u,  986u, 1046u, 1155u, 1253u,
    1297u, 1379u, 1452u, 1486u, 1547u, 1602u, 1650u, 1673u, 1714u, 1750u,
    1767u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u, 1907u, 1923u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1253u, 1297u, 1379u, 1452u, 1486u, 1517u, 1547u, 1602u, 1650u,
    1673u, 1714u, 1750u, 1767u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u,
    1907u, 1923u, 1936u, 1949u, 1954u, 1964u, 1974u, 1978u, 1985u,
};

static const uint8_t sunset_lead_seq[SUNSET_FIELD_LEN] = {
    19,  0, 21,  0, 22,  0, 23,  0, 24, 23, 22,  0, 21,  0, 19,  0, 18,  0, 19,  0, 21,  0, 22,  0, 23, 22, 21,  0, 19,  0, 18,  0,
    17,  0, 19,  0, 21,  0, 22,  0, 23,  0, 24, 23, 22,  0, 21,  0, 19,  0, 21, 22, 23,  0, 22, 21, 19,  0, 18,  0, 19,  0,  0,  0,
    19, 21, 22,  0, 23,  0, 24, 23, 22, 21, 19,  0, 21,  0, 22,  0, 23,  0, 24, 25, 26,  0, 25, 24, 23, 22, 21,  0, 19,  0, 21,  0,
    22,  0, 23, 24, 25,  0, 24, 23, 22, 21, 19,  0, 18,  0, 19, 21, 22, 23, 24,  0, 23, 22, 21,  0, 19,  0, 18,  0, 19,  0,  0,  0,
    24,  0, 23, 22, 21,  0, 22, 23, 24, 25, 26,  0, 25, 24, 23,  0, 22,  0, 23, 24, 25,  0, 26,  0, 25, 24, 23, 22, 21,  0, 19,  0,
    21,  0, 22, 23, 24,  0, 25,  0, 26, 25, 24, 23, 22, 21, 19,  0, 18, 19, 21,  0, 22, 23, 24,  0, 23, 22, 21,  0, 19,  0,  0,  0,
    19, 21, 23,  0, 26,  0, 25, 24, 23,  0, 24, 25, 26,  0, 28,  0, 26,  0, 25, 24, 23, 22, 21,  0, 22,  0, 23, 24, 23,  0, 21,  0,
    24,  0, 23, 22, 21,  0, 22, 23, 24,  0, 26, 25, 24, 23, 22,  0, 21, 22, 23,  0, 22, 21, 19,  0, 18,  0, 19, 21, 19,  0,  0,  0,
    19,  0, 21,  0, 22,  0, 23,  0, 24, 23, 22,  0, 21,  0, 19,  0, 18,  0, 19,  0, 21,  0, 22,  0, 23, 22, 21,  0, 19,  0, 18,  0,
    17,  0, 19,  0, 21,  0, 22,  0, 23,  0, 24, 23, 22,  0, 21,  0, 19,  0, 21, 22, 23,  0, 22, 21, 19,  0, 18,  0, 19,  0,  0,  0,
    21,  0, 22, 23, 24,  0, 23, 22, 21,  0, 19, 21, 22,  0, 23,  0, 24,  0, 25, 26, 25, 24, 23,  0, 22, 21, 19,  0, 18,  0, 19,  0,
    19, 21, 22, 23, 24, 23, 22, 21, 19,  0, 21, 22, 23,  0, 24,  0, 23, 22, 21,  0, 19,  0, 18,  0, 17,  0, 18,  0, 19,  0,  0,  0,
};

static const uint8_t sunset_harmony_seq[SUNSET_FIELD_LEN] = {
    16,  0, 19,  0, 21,  0, 19,  0, 22, 21, 19,  0, 16,  0, 14,  0, 15,  0, 16,  0, 19,  0, 21,  0, 22, 21, 19,  0, 16,  0, 15,  0,
    14,  0, 17,  0, 19,  0, 21,  0, 22,  0, 21, 19, 17,  0, 16,  0, 12,  0, 16, 17, 18,  0, 17, 16, 14,  0, 15,  0, 16,  0,  0,  0,
    16, 19, 21,  0, 21,  0, 22, 21, 19, 16, 17,  0, 19,  0, 21,  0, 21,  0, 22, 23, 24,  0, 23, 22, 21, 19, 16,  0, 14,  0, 16,  0,
    17,  0, 18, 19, 21,  0, 19, 18, 17, 16, 14,  0, 15,  0, 16, 19, 17, 18, 19,  0, 18, 17, 16,  0, 14,  0, 15,  0, 16,  0,  0,  0,
    21,  0, 19, 17, 16,  0, 17, 19, 21, 22, 23,  0, 22, 21, 19,  0, 17,  0, 18, 19, 21,  0, 23,  0, 22, 21, 19, 17, 16,  0, 14,  0,
    16,  0, 17, 18, 19,  0, 21,  0, 23, 22, 21, 19, 17, 16, 14,  0, 15, 16, 19,  0, 21, 22, 21,  0, 19, 17, 16,  0, 14,  0,  0,  0,
    16, 19, 21,  0, 23,  0, 22, 21, 19,  0, 21, 22, 23,  0, 26,  0, 23,  0, 22, 21, 19, 17, 16,  0, 17,  0, 18, 19, 18,  0, 16,  0,
    21,  0, 19, 17, 16,  0, 17, 19, 21,  0, 23, 22, 21, 19, 17,  0, 16, 17, 18,  0, 17, 16, 14,  0, 15,  0, 16, 19, 16,  0,  0,  0,
    16,  0, 19,  0, 21,  0, 19,  0, 22, 21, 19,  0, 16,  0, 14,  0, 15,  0, 16,  0, 19,  0, 21,  0, 22, 21, 19,  0, 16,  0, 15,  0,
    14,  0, 17,  0, 19,  0, 21,  0, 22,  0, 21, 19, 17,  0, 16,  0, 12,  0, 16, 17, 18,  0, 17, 16, 14,  0, 15,  0, 16,  0,  0,  0,
    16,  0, 17, 18, 19,  0, 18, 17, 16,  0, 14, 16, 17,  0, 18,  0, 19,  0, 21, 23, 22, 21, 19,  0, 17, 16, 14,  0, 15,  0, 16,  0,
    16, 19, 21, 22, 21, 19, 17, 16, 14,  0, 16, 17, 18,  0, 19,  0, 18, 17, 16,  0, 14,  0, 15,  0, 12,  0, 15,  0, 16,  0,  0,  0,
};

static const uint8_t sunset_bass_seq[SUNSET_FIELD_LEN] = {
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,
     2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  7,  0,  7,  0, 11,  0,  7,  0,  7,  0, 11,  0,  7,  0,  7,  0,
     2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,
     2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  8,  0,  8,  0, 12,  0,  8,  0,  8,  0, 12,  0,  8,  0,  8,  0,
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,
     2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,  6,  0,  1,  0,  1,  0,
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,  8,  0,  3,  0,  3,  0,
     2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,
     7,  0,  7,  0, 11,  0,  7,  0,  7,  0, 11,  0,  7,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,  7,  0,  2,  0,  2,  0,
     4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,  9,  0,  4,  0,  4,  0,
};


static uint16_t sunset_pos;
static uint8_t sunset_tick;
static uint8_t sunset_playing;
static uint8_t sunset_ch3_loaded;

static void load_sunset_wave(void) {
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
    sunset_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u;
    NR12_REG = 0x74u; /* softer, singing envelope */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_harmony_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0x40u;
    NR22_REG = 0x52u;
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!sunset_ch3_loaded) {
        load_sunset_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_soft_noise(uint8_t kind) {
    NR41_REG = 0x20u;

    if (kind == 2u) {
        /* distant low pulse */
        NR42_REG = 0x41u;
        NR43_REG = 0x65u;
    } else if (kind == 1u) {
        /* soft wind/cymbal */
        NR42_REG = 0x32u;
        NR43_REG = 0x46u;
    } else {
        /* faint tick */
        NR42_REG = 0x21u;
        NR43_REG = 0x67u;
    }

    NR44_REG = 0x80u;
}

static void sunset_play_step(uint16_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);

    trigger_lead_ch1(pulse_freq_table[sunset_lead_seq[pos]]);
    trigger_harmony_ch2(pulse_freq_table[sunset_harmony_seq[pos]]);

    /* Keep the bass broad and less clicky. */
    set_bass_ch3(wave_freq_table[sunset_bass_seq[pos]], ((beat & 7u) == 0u));

    if (beat == 0u) {
        trigger_soft_noise(2u);
    } else if (beat == 8u) {
        trigger_soft_noise(1u);
    } else if ((beat == 6u) || (beat == 14u)) {
        trigger_soft_noise(0u);
    }
}

void sunset_ruins_field_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    sunset_ch3_loaded = 0u;
    load_sunset_wave();

    sunset_pos = 0u;
    sunset_tick = 0u;
    sunset_playing = 1u;

    sunset_play_step(0u);
}

void sunset_ruins_field_bgm_update(void) BANKED {
    if (!sunset_playing) return;

    ++sunset_tick;
    if (sunset_tick >= SUNSET_FIELD_STEP_FRAMES) {
        sunset_tick = 0u;
        ++sunset_pos;

        if (sunset_pos >= SUNSET_FIELD_LEN) {
            sunset_pos = 0u;
        }

        sunset_play_step(sunset_pos);
    }
}

void sunset_ruins_field_bgm_stop(void) BANKED {
    sunset_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
