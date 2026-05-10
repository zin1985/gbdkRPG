#pragma bank 12

#include "peaceful_balanced_town_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define PEACEFUL_BALANCED_TOWN_LEN 384u
#define PEACEFUL_BALANCED_TOWN_STEP_FRAMES 10u

/*
 * Original peaceful balanced town BGM for Game Boy / GBDK.
 *
 * Design goals:
 * - Peaceful town music.
 * - Avoid pentatonic-only / East Asian-sounding contour by using clear 7-note major scale motion.
 * - Use B->C and F->E resolution, plus C/F/G/Am/Dm/G7-style functional harmony.
 * - Add sustained string-like notes and rhythmic contrast.
 * - Add a gentle second-half lift while keeping the mood calm.
 *
 * Channels:
 * - CH1: sustained main melody
 * - CH2: sustained string harmony
 * - CH3: warm bass/pad
 * - CH4: very light rhythm/air
 */

static const uint16_t pulse_freq_table[] = {
       0u,  547u,  711u,  856u, 1046u, 1155u, 1253u, 1379u, 1452u, 1602u,
    1650u, 1714u, 1750u, 1783u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u,
    1915u, 1923u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1297u, 1379u, 1452u, 1547u, 1602u, 1650u, 1714u, 1750u, 1825u,
    1849u, 1881u, 1899u, 1915u, 1923u, 1936u, 1949u, 1954u, 1964u, 1974u,
    1982u, 1985u,
};

static const uint8_t balanced_town_lead_seq[PEACEFUL_BALANCED_TOWN_LEN] = {
    14, 14, 14, 14, 16, 16, 17, 16, 15, 15, 15, 15, 13, 14, 15,  0, 16, 16, 16, 16, 15, 15, 14, 13, 14, 14, 14, 14, 11, 11, 11, 11,
    12, 12, 12, 13, 14, 14, 14, 14, 17, 17, 16, 15, 14, 14, 14, 14, 15, 15, 16, 17, 16, 16, 16, 16, 15, 15, 13, 14, 14, 14, 14, 14,
    14, 14, 14, 15, 16, 16, 16, 17, 18, 18, 18, 18, 16, 16, 16, 16, 17, 17, 16, 15, 14, 14, 14, 14, 15, 15, 16, 17, 16, 16, 16, 16,
    12, 12, 14, 15, 16, 16, 16, 16, 17, 17, 16, 15, 14, 14, 14, 14, 13, 13, 14, 15, 14, 14, 14, 14, 14, 14, 11, 11, 14, 14,  0,  0,
    18, 18, 18, 18, 17, 17, 16, 15, 16, 16, 16, 16, 14, 14, 14, 14, 17, 17, 17, 18, 19, 19, 19, 19, 18, 18, 17, 16, 15, 15, 15, 15,
    16, 16, 16, 16, 17, 17, 18, 16, 15, 15, 15, 15, 14, 14, 14, 14, 15, 15, 16, 17, 18, 18, 18, 18, 17, 17, 16, 15, 14, 14, 14, 14,
    14, 14, 14, 14, 16, 16, 16, 16, 18, 18, 18, 18, 21, 21, 21, 21, 20, 20, 19, 18, 17, 17, 17, 17, 16, 16, 15, 14, 15, 15, 15, 15,
    16, 16, 16, 17, 18, 18, 18, 19, 18, 18, 18, 18, 16, 16, 16, 16, 17, 17, 16, 15, 14, 14, 14, 14, 13, 13, 14, 15, 14, 14, 14, 14,
    16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 15, 15, 15, 16, 17, 17, 17, 17, 16, 16, 15, 14, 13, 13, 13, 13,
    14, 14, 14, 14, 16, 16, 16, 16, 17, 17, 16, 15, 14, 14, 14, 14, 12, 12, 12, 12, 11, 11, 11, 11, 13, 13, 13, 13, 14, 14, 14, 14,
    14, 14, 14, 14, 16, 16, 17, 16, 15, 15, 15, 15, 13, 14, 15,  0, 16, 16, 16, 16, 15, 15, 14, 13, 14, 14, 14, 14, 11, 11, 11, 11,
    12, 12, 12, 13, 14, 14, 14, 14, 17, 17, 16, 15, 14, 14, 14, 14, 15, 15, 16, 17, 16, 16, 15, 13, 14, 14, 14, 14, 14, 14, 14, 14,
};

static const uint8_t balanced_town_harmony_seq[PEACEFUL_BALANCED_TOWN_LEN] = {
    11, 11, 11, 11, 14, 14, 15, 14, 13, 13, 13, 13, 11, 12, 13,  0, 14, 14, 14, 14, 13, 13, 12, 11, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 11, 12, 12, 12, 12, 14, 14, 13, 12, 11, 11, 11, 11, 12, 12, 13, 14, 14, 14, 14, 14, 13, 13, 11, 12, 11, 11, 11, 11,
    11, 11, 11, 12, 14, 14, 14, 15, 16, 16, 16, 16, 14, 14, 14, 14, 15, 15, 14, 13, 12, 12, 12, 12, 13, 13, 14, 15, 14, 14, 14, 14,
    10, 10, 11, 12, 14, 14, 14, 14, 15, 15, 14, 13, 12, 12, 12, 12, 11, 11, 12, 13, 11, 11, 11, 11, 11, 11, 10, 10, 11, 11,  0,  0,
    16, 16, 16, 16, 15, 15, 14, 13, 14, 14, 14, 14, 11, 11, 11, 11, 15, 15, 15, 16, 17, 17, 17, 17, 16, 16, 15, 14, 12, 12, 12, 12,
    14, 14, 14, 14, 15, 15, 16, 14, 13, 13, 13, 13, 11, 11, 11, 11, 13, 13, 14, 15, 16, 16, 16, 16, 15, 15, 14, 13, 11, 11, 11, 11,
    11, 11, 11, 11, 14, 14, 14, 14, 16, 16, 16, 16, 18, 18, 18, 18, 18, 18, 17, 16, 15, 15, 15, 15, 14, 14, 13, 12, 13, 13, 13, 13,
    14, 14, 14, 15, 16, 16, 16, 17, 16, 16, 16, 16, 14, 14, 14, 14, 15, 15, 14, 13, 12, 12, 12, 12, 11, 11, 12, 13, 11, 11, 11, 11,
    14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 10, 10, 10, 10, 12, 12, 12, 13, 14, 14, 14, 14, 13, 13, 12, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 14, 14, 14, 14, 15, 15, 14, 13, 11, 11, 11, 11, 10, 10, 10, 10,  9,  9,  9,  9, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 14, 14, 15, 14, 13, 13, 13, 13, 11, 12, 13,  0, 14, 14, 14, 14, 13, 13, 12, 11, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 11, 12, 12, 12, 12, 14, 14, 13, 12, 11, 11, 11, 11, 12, 12, 13, 14, 14, 14, 13, 11, 11, 11, 11, 11, 11, 11, 11, 11,
};

static const uint8_t balanced_town_bass_seq[PEACEFUL_BALANCED_TOWN_LEN] = {
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  4,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  2,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  3,  0,  1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  1,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  4,  0,  1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  1,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  3,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  2,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  4,  0,  1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  1,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  3,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  2,  0,
     4,  0,  0,  0,  7,  0,  4,  0,  7,  0,  0,  0,  4,  0,  7,  0,  2,  0,  0,  0,  5,  0,  2,  0,  5,  0,  0,  0,  2,  0,  5,  0,
     3,  0,  0,  0,  6,  0,  3,  0,  6,  0,  0,  0,  3,  0,  6,  0,  1,  0,  0,  0,  4,  0,  1,  0,  4,  0,  0,  0,  1,  0,  4,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  3,  0,  5,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  5,  0,
     1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  1,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  2,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  4,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  2,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  3,  0,  4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  4,  0,
};


static uint16_t balanced_pos = 0u;
static uint8_t balanced_tick = 0u;
static uint8_t balanced_playing = 0u;
static uint8_t balanced_ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;
static uint8_t last_harmony_note = 255u;

static void load_balanced_wave(void) {
    static const uint8_t wave_data[16] = {
        0x8Au,0xBCu,0xDEu,0xFFu,0xEDu,0xBAu,0x86u,0x53u,
        0x35u,0x68u,0xABu,0xDEu,0xFEu,0xDCu,0xA8u,0x75u
    };

    uint8_t i;
    NR30_REG = 0x00u;
    for (i = 0u; i != 16u; ++i) {
        *((uint8_t *)0xFF30u + i) = wave_data[i];
    }
    NR30_REG = 0x80u;
    balanced_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u; /* smooth 50% pulse */
    NR12_REG = 0x60u; /* steady soft volume */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_harmony_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0xC0u; /* rounder harmony */
    NR22_REG = 0x40u; /* quiet steady harmony */
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!balanced_ch3_loaded) {
        load_balanced_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_soft_air_ch4(uint8_t kind) {
    NR41_REG = 0x20u;

    if (kind == 2u) {
        NR42_REG = 0x21u;
        NR43_REG = 0x67u;
    } else if (kind == 1u) {
        NR42_REG = 0x11u;
        NR43_REG = 0x77u;
    } else {
        NR42_REG = 0x00u;
    }

    NR44_REG = 0x80u;
}

static void balanced_town_play_step(uint16_t pos) {
    uint8_t step = (uint8_t)(pos & 15u);
    uint8_t lead_note = balanced_town_lead_seq[pos];
    uint8_t harmony_note = balanced_town_harmony_seq[pos];

    /* Sustain repeated notes for string-like phrasing. */
    if (lead_note != last_lead_note) {
        trigger_lead_ch1(pulse_freq_table[lead_note]);
        last_lead_note = lead_note;
    }

    if (harmony_note != last_harmony_note) {
        trigger_harmony_ch2(pulse_freq_table[harmony_note]);
        last_harmony_note = harmony_note;
    }

    /* Warm bass pad. */
    set_bass_ch3(wave_freq_table[balanced_town_bass_seq[pos]], ((step == 0u) || (step == 8u)));

    /* Very light pulse, only to give rhythm a little shape. */
    if (step == 0u) {
        trigger_soft_air_ch4(2u);
    } else if ((step == 6u) || (step == 10u)) {
        trigger_soft_air_ch4(1u);
    }
}

void peaceful_balanced_town_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x66u;
    NR51_REG = 0xFFu;

    balanced_ch3_loaded = 0u;
    load_balanced_wave();

    balanced_pos = 0u;
    balanced_tick = 0u;
    balanced_playing = 1u;
    last_lead_note = 255u;
    last_harmony_note = 255u;

    balanced_town_play_step(0u);
}

void peaceful_balanced_town_bgm_update(void) BANKED {
    if (!balanced_playing) return;

    ++balanced_tick;
    if (balanced_tick >= PEACEFUL_BALANCED_TOWN_STEP_FRAMES) {
        balanced_tick = 0u;
        ++balanced_pos;

        if (balanced_pos >= PEACEFUL_BALANCED_TOWN_LEN) {
            balanced_pos = 0u;
            last_lead_note = 255u;
            last_harmony_note = 255u;
        }

        balanced_town_play_step(balanced_pos);
    }
}

void peaceful_balanced_town_bgm_stop(void) BANKED {
    balanced_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
