#pragma bank 12
#include "gentle_strings_town_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define GENTLE_STRINGS_TOWN_LEN 320u
#define GENTLE_STRINGS_TOWN_STEP_FRAMES 14u

/*
 * Original gentle strings town BGM for Game Boy / GBDK.
 *
 * Mood:
 * - calm town, afternoon light, quiet inn, warm home base
 * - slower and more string-centered than bright_classic_town_bgm
 * - no busy harp pattern, no strong percussion
 *
 * Important:
 * - The Game Boy cannot play real strings.
 * - This player simulates string-like sustain by not retriggering CH1/CH2
 *   when the same note continues.
 *
 * Channels:
 * - CH1: sustained main string-like melody
 * - CH2: sustained harmony strings
 * - CH3: warm bass/pad
 * - CH4: very soft room air
 */

static const uint16_t pulse_freq_table[] = {
       0u,  547u,  711u,  856u, 1046u, 1155u, 1253u, 1379u, 1452u, 1602u,
    1650u, 1714u, 1750u, 1783u, 1798u, 1825u, 1849u, 1860u, 1881u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1297u, 1379u, 1452u, 1547u, 1602u, 1650u, 1714u, 1750u, 1825u,
    1849u, 1881u, 1899u, 1915u, 1923u, 1936u, 1949u, 1954u, 1964u,
};

static const uint8_t gentle_town_lead_seq[GENTLE_STRINGS_TOWN_LEN] = {
    14, 14, 14, 14, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16,
    17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 11, 11, 11, 11,
    14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 18, 18, 18, 18, 17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14,
    12, 12, 12, 12, 14, 14, 14, 14, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14, 11, 11, 11, 11, 14, 14, 14, 14,
    16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 16, 16, 16, 16, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 15, 15, 15, 15,
    14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16,
    18, 18, 18, 18, 17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12,
    14, 14, 14, 14, 16, 16, 16, 16, 17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 11, 11, 11, 11, 14, 14, 14, 14,
    14, 14, 14, 14, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16,
    17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 11, 11, 11, 11, 14, 14, 14, 14,
};

static const uint8_t gentle_town_harmony_seq[GENTLE_STRINGS_TOWN_LEN] = {
    11, 11, 11, 11, 14, 14, 14, 14, 13, 13, 13, 13, 11, 11, 11, 11, 10, 10, 10, 10, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14,
    14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10,  9,  9,  9,  9,
    11, 11, 11, 11, 12, 12, 12, 12, 14, 14, 14, 14, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 11, 11, 11, 11,
    10, 10, 10, 10, 11, 11, 11, 11, 14, 14, 14, 14, 13, 13, 13, 13, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11,
    14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 12, 12, 12, 12,
    11, 11, 11, 11, 12, 12, 12, 12, 14, 14, 14, 14, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 14, 14, 14, 14,
    16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 14, 14, 14, 14, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10,
    11, 11, 11, 11, 14, 14, 14, 14, 15, 15, 15, 15, 14, 14, 14, 14, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11,
    11, 11, 11, 11, 14, 14, 14, 14, 13, 13, 13, 13, 11, 11, 11, 11, 10, 10, 10, 10, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14,
    14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11, 11, 11,
};

static const uint8_t gentle_town_bass_seq[GENTLE_STRINGS_TOWN_LEN] = {
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,
     1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,
     3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,
     1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,
     1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,
     4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,
     1,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,
};


static uint16_t gentle_pos = 0u;
static uint8_t gentle_tick = 0u;
static uint8_t gentle_playing = 0u;
static uint8_t gentle_ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;
static uint8_t last_harmony_note = 255u;

static void load_gentle_wave(void) {
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
    gentle_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u; /* 50% duty, smooth string-like pulse */
    NR12_REG = 0x60u; /* steady soft volume, no envelope sweep */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_harmony_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0xC0u; /* rounder harmony */
    NR22_REG = 0x40u; /* steady quiet volume */
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!gentle_ch3_loaded) {
        load_gentle_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_soft_air_ch4(uint8_t kind) {
    NR41_REG = 0x20u;

    if (kind == 1u) {
        NR42_REG = 0x11u;
        NR43_REG = 0x77u;
    } else {
        NR42_REG = 0x00u;
    }

    NR44_REG = 0x80u;
}

static void gentle_town_play_step(uint16_t pos) {
    uint8_t step = (uint8_t)(pos & 15u);
    uint8_t lead_note = gentle_town_lead_seq[pos];
    uint8_t harmony_note = gentle_town_harmony_seq[pos];

    /* Sustained string-like melody. */
    if (lead_note != last_lead_note) {
        trigger_lead_ch1(pulse_freq_table[lead_note]);
        last_lead_note = lead_note;
    }

    /* Sustained string-like harmony. */
    if (harmony_note != last_harmony_note) {
        trigger_harmony_ch2(pulse_freq_table[harmony_note]);
        last_harmony_note = harmony_note;
    }

    /* Warm bass pad. */
    set_bass_ch3(wave_freq_table[gentle_town_bass_seq[pos]], ((step == 0u) || (step == 8u)));

    /* Almost silent air, not percussion. */
    if ((step == 0u) || (step == 8u)) {
        trigger_soft_air_ch4(1u);
    }
}

void gentle_strings_town_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x66u;
    NR51_REG = 0xFFu;

    gentle_ch3_loaded = 0u;
    load_gentle_wave();

    gentle_pos = 0u;
    gentle_tick = 0u;
    gentle_playing = 1u;
    last_lead_note = 255u;
    last_harmony_note = 255u;

    gentle_town_play_step(0u);
}

void gentle_strings_town_bgm_update(void) BANKED {
    if (!gentle_playing) return;

    ++gentle_tick;
    if (gentle_tick >= GENTLE_STRINGS_TOWN_STEP_FRAMES) {
        gentle_tick = 0u;
        ++gentle_pos;

        if (gentle_pos >= GENTLE_STRINGS_TOWN_LEN) {
            gentle_pos = 0u;
            last_lead_note = 255u;
            last_harmony_note = 255u;
        }

        gentle_town_play_step(gentle_pos);
    }
}

void gentle_strings_town_bgm_stop(void) BANKED {
    gentle_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
