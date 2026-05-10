#pragma bank 15

#include "heavy_metal_celtic_battle_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define MELODIC_ROCK_STRINGS_FINALE_LEN 256u
#define MELODIC_ROCK_STRINGS_FINALE_STEP_FRAMES 6u
#define MELODIC_ROCK_DROP_POS 64u
#define STRINGS_FINALE_POS 192u

/*
 * Original melodic rock battle BGM with quiet strings finale.
 *
 * Structure:
 * - 0-63: preserved intro phrase
 * - 64-191: melodic rock battle section with strong drums
 * - 192-255: quiet, sorrowful, brave string-like melody + drums only
 *
 * Channels:
 * - CH1: lead / finale strings
 * - CH2: rhythm guitar / finale string harmony
 * - CH3: bass before finale, silent in finale
 * - CH4: drums/noise
 */

static const uint16_t pulse_freq_table[] = {
       0u,   44u,  263u,  458u,  547u,  711u,  856u,  923u,  986u, 1046u,
    1155u, 1253u, 1297u, 1379u, 1452u, 1486u, 1517u, 1547u, 1602u, 1650u,
    1673u, 1714u, 1750u, 1767u, 1783u, 1798u, 1825u, 1849u, 1860u, 1881u,
    1899u, 1907u, 1915u, 1923u, 1936u, 1949u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1046u, 1155u, 1253u, 1297u, 1379u, 1452u, 1486u, 1517u, 1547u,
    1602u, 1650u, 1673u, 1714u, 1750u, 1767u, 1783u, 1798u, 1825u, 1849u,
    1860u, 1881u, 1899u, 1907u, 1915u, 1923u, 1936u, 1949u, 1954u, 1964u,
    1974u, 1978u, 1982u, 1985u, 1992u, 1998u,
};

static const uint8_t finale_lead_seq[MELODIC_ROCK_STRINGS_FINALE_LEN] = {
    26,  0, 26, 28, 30,  0, 29, 28, 27,  0, 28, 29, 30,  0, 34,  0, 33,  0, 30, 29, 28,  0, 26, 28, 27,  0, 25, 26, 28,  0, 30,  0,
    31,  0, 30, 29, 30,  0, 34, 33, 30,  0, 29, 28, 27,  0, 26,  0, 28, 29, 30,  0, 33, 30, 29,  0, 28, 27, 26,  0, 26,  0, 22,  0,
    26, 28, 30, 34, 33, 30, 29, 28, 27, 28, 29, 30, 29, 28, 27, 26, 28, 30, 33, 30, 29, 28, 27, 26, 27, 28, 29, 30, 33, 30, 29, 28,
    29, 30, 31, 30, 29, 28, 27, 26, 25, 26, 27, 28, 29, 30, 29, 28, 27, 28, 29, 30, 33, 30, 29, 28, 27, 26, 25, 22, 26,  0, 22,  0,
    30, 33, 34, 33, 30, 29, 28, 27, 28, 29, 30, 29, 28, 27, 26, 25, 26, 27, 28, 29, 30, 33, 30, 29, 28, 27, 26, 25, 26, 28, 30, 33,
    31, 30, 29, 28, 27, 28, 29, 30, 33, 30, 29, 28, 27, 26, 25, 22, 26, 28, 30, 33, 34, 33, 30, 29, 28, 27, 26, 25, 26,  0, 26,  0,
    34, 34, 34, 34, 33, 33, 33, 33, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 30, 30, 30, 30, 29, 29, 29, 29, 27, 27, 27, 27,
    26, 26, 26, 26, 28, 28, 28, 28, 30, 30, 30, 30, 33, 33, 33, 33, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 26, 26, 26, 26,
};

static const uint8_t finale_guitar_seq[MELODIC_ROCK_STRINGS_FINALE_LEN] = {
    22,  0, 22, 25, 26,  0, 25, 22, 21,  0, 22, 25, 26,  0, 28,  0, 27,  0, 25, 22, 26,  0, 22, 26, 25,  0, 21, 22, 25,  0, 26,  0,
    28,  0, 27, 26, 27,  0, 30, 29, 27,  0, 26, 25, 22,  0, 20,  0, 22, 25, 26,  0, 27, 26, 25,  0, 22, 21, 20,  0, 20,  0, 19,  0,
    18, 18,  0, 22, 18, 18,  0, 26, 18, 18,  0, 22, 26, 28, 26, 22, 15, 15,  0, 20, 15, 15,  0, 23, 15, 15,  0, 20, 23, 26, 23, 20,
    20, 20,  0, 25, 20, 20,  0, 28, 20, 20,  0, 25, 28, 30, 28, 25, 17, 17,  0, 21, 17, 17,  0, 25, 17, 17,  0, 21, 25, 27, 25, 21,
    13, 13,  0, 18, 13, 13,  0, 21, 13, 13,  0, 18, 21, 23, 21, 18, 14, 14,  0, 19, 14, 14,  0, 22, 14, 14,  0, 19, 22, 25, 22, 19,
    18, 18,  0, 22, 18, 18,  0, 26, 18, 18,  0, 22, 26, 28, 26, 22, 15, 15,  0, 20, 15, 15,  0, 23, 15, 15,  0, 20, 23, 26, 23, 20,
    30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 28, 28, 28, 28, 27, 27, 27, 27, 25, 25, 25, 25,
    22, 22, 22, 22, 26, 26, 26, 26, 28, 28, 28, 28, 30, 30, 30, 30, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 22, 22, 22, 22,
};

static const uint8_t finale_bass_seq[MELODIC_ROCK_STRINGS_FINALE_LEN] = {
    10, 10, 14, 10, 10, 14, 10, 14, 10, 10, 14, 10, 14, 10, 14, 10,  9,  9, 13,  9,  9, 13,  9, 13,  9,  9, 13,  9, 13,  9, 13,  9,
     7,  7, 12,  7,  7, 12,  7, 12,  7,  7, 12,  7, 12,  7, 12,  7, 10, 10, 14, 10, 10, 14, 10, 14, 10, 10, 14, 10, 14, 10, 14, 10,
     2,  2,  2,  6,  2,  2,  2,  6,  2,  2,  2,  6,  2,  6,  2,  6,  2,  2,  2,  6,  2,  2,  2,  6,  2,  2,  2,  6,  2,  6,  2,  6,
     7,  7,  7,  4,  7,  7,  7,  4,  7,  7,  7,  4,  7,  4,  7,  4,  4,  4,  4,  9,  4,  4,  4,  9,  4,  4,  4,  9,  4,  9,  4,  9,
     1,  1,  1,  5,  1,  1,  1,  5,  1,  1,  1,  5,  1,  5,  1,  5,  1,  1,  1,  5,  1,  1,  1,  5,  1,  1,  1,  5,  1,  5,  1,  5,
     5,  5,  5, 10,  5,  5,  5, 10,  5,  5,  5, 10,  5, 10,  5, 10,  6,  6,  6, 11,  6,  6,  6, 11,  6,  6,  6, 11,  6, 11,  6, 11,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};


static uint16_t song_pos;
static uint8_t song_tick;
static uint8_t song_playing;
static uint8_t ch3_loaded;
static uint8_t last_lead_note;
static uint8_t last_harmony_note;

static void load_wave(void) {
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
    ch3_loaded = 1u;
}

static void trigger_ch1(uint16_t f, uint8_t drop, uint8_t finale) {
    if (f == 0u) { NR12_REG = 0x00u; return; }
    NR10_REG = 0x00u;
    NR11_REG = finale ? 0x80u : (drop ? 0x80u : 0xC0u);
    NR12_REG = finale ? 0x64u : (drop ? 0x83u : 0x84u);
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_ch2(uint16_t f, uint8_t drop, uint8_t finale) {
    if (f == 0u) { NR22_REG = 0x00u; return; }
    NR21_REG = finale ? 0xC0u : (drop ? 0x40u : 0x80u);
    NR22_REG = finale ? 0x44u : (drop ? 0x72u : 0x53u);
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) { NR30_REG = 0x00u; return; }
    if (!ch3_loaded) load_wave();
    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_drum(uint8_t kind, uint8_t drop, uint8_t finale) {
    NR41_REG = finale ? 0x18u : (drop ? 0x08u : 0x18u);

    if (kind == 3u) {
        NR42_REG = finale ? 0x21u : 0x31u;
        NR43_REG = finale ? 0x47u : 0x26u;
    } else if (kind == 2u) {
        NR42_REG = finale ? 0x62u : (drop ? 0x84u : 0x73u);
        NR43_REG = finale ? 0x35u : (drop ? 0x34u : 0x35u);
    } else if (kind == 1u) {
        NR42_REG = finale ? 0x71u : (drop ? 0x91u : 0x81u);
        NR43_REG = finale ? 0x56u : (drop ? 0x56u : 0x57u);
    } else {
        NR42_REG = 0x31u;
        NR43_REG = 0x47u;
    }

    NR44_REG = 0x80u;
}

static void play_step(uint16_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t drop = ((pos >= MELODIC_ROCK_DROP_POS) && (pos < STRINGS_FINALE_POS));
    uint8_t finale = (pos >= STRINGS_FINALE_POS);
    uint8_t lead_note = finale_lead_seq[pos];
    uint8_t harmony_note = finale_guitar_seq[pos];

    if (finale) {
        /* In finale, sustain repeated notes for string-like phrasing. */
        if (lead_note != last_lead_note) {
            trigger_ch1(pulse_freq_table[lead_note], drop, finale);
            last_lead_note = lead_note;
        }
        if (harmony_note != last_harmony_note) {
            trigger_ch2(pulse_freq_table[harmony_note], drop, finale);
            last_harmony_note = harmony_note;
        }
        NR30_REG = 0x00u; /* drums + strings only */
    } else {
        last_lead_note = 255u;
        last_harmony_note = 255u;
        trigger_ch1(pulse_freq_table[lead_note], drop, finale);
        trigger_ch2(pulse_freq_table[harmony_note], drop, finale);
        set_ch3(wave_freq_table[finale_bass_seq[pos]], drop ? 1u : ((beat & 3u) == 0u));
    }

    if (finale) {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, drop, finale);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, drop, finale);
        else if ((beat == 6u) || (beat == 14u)) trigger_drum(3u, drop, finale);
    } else if (drop) {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, drop, finale);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, drop, finale);
        else if ((beat == 2u) || (beat == 6u) || (beat == 10u) || (beat == 14u)) trigger_drum(3u, drop, finale);
        else trigger_drum(0u, drop, finale);
    } else {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, drop, finale);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, drop, finale);
        else if ((beat == 7u) || (beat == 15u)) trigger_drum(0u, drop, finale);
    }
}

void heavy_metal_celtic_battle_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    ch3_loaded = 0u;
    load_wave();

    song_pos = 0u;
    song_tick = 0u;
    song_playing = 1u;
    last_lead_note = 255u;
    last_harmony_note = 255u;

    play_step(0u);
}

void heavy_metal_celtic_battle_bgm_update(void) BANKED {
    if (!song_playing) return;

    ++song_tick;
    if (song_tick >= MELODIC_ROCK_STRINGS_FINALE_STEP_FRAMES) {
        song_tick = 0u;
        ++song_pos;

        if (song_pos >= MELODIC_ROCK_STRINGS_FINALE_LEN) {
            song_pos = 0u;
            last_lead_note = 255u;
            last_harmony_note = 255u;
        }

        play_step(song_pos);
    }
}

void heavy_metal_celtic_battle_bgm_stop(void) BANKED {
    song_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
