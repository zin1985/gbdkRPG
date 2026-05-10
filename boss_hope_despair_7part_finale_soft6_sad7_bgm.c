#pragma bank 9

#include "boss_hope_despair_7part_finale_soft6_sad7_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define BOSS_HOPE_DESPAIR_SOFT6_SAD7_LEN 448u
#define BOSS_HOPE_DESPAIR_SOFT6_SAD7_STEP_FRAMES 6u
#define PART2_POS 64u
#define PART3_POS 128u
#define PIPE_ORGAN_HYMN_POS 192u
#define FINAL_MAJOR_CADENCE_POS 304u
#define PART6_CONFLICT_POS 320u
#define PART7_FINAL_POS 384u

/*
 * Original boss BGM with restrained Part 6 and sad rhythmic Part 7 finale.
 *
 * Structure:
 * 1. 0-63    : low despair
 * 2. 64-127  : sorrowful melodic rock
 * 3. 128-191 : sorrowful melodic rock with pressure
 * 4. 192-255 : pipe-organ hymn-like minor section
 * 5. 256-319 : pipe-organ hymn-like section, final D major cadence
 * 6. 320-383 : restrained D major / minor-shadow conflict, less bouncy
 * 7. 384-447 : sad, emotional, rhythmic pipe-organ hymn closure
 */

static const uint16_t pulse_freq_table[] = {
       0u,   44u,  263u,  547u,  711u,  856u,  923u, 1046u, 1155u, 1253u,
    1297u, 1379u, 1452u, 1486u, 1547u, 1602u, 1627u, 1650u, 1673u, 1714u,
    1732u, 1750u, 1767u, 1783u, 1798u, 1812u, 1825u, 1849u, 1860u, 1871u,
    1881u, 1899u, 1907u, 1915u, 1923u, 1930u, 1936u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1046u, 1155u, 1297u, 1379u, 1452u, 1486u, 1547u, 1602u, 1650u,
    1673u, 1714u, 1750u, 1767u, 1798u, 1825u, 1837u, 1849u, 1860u, 1881u,
    1890u, 1899u, 1907u, 1915u, 1923u, 1930u, 1936u, 1949u, 1954u, 1959u,
    1964u, 1974u, 1978u, 1982u, 1985u, 1989u, 1992u,
};

static const uint8_t soft7_lead_seq[BOSS_HOPE_DESPAIR_SOFT6_SAD7_LEN] = {
    15,  0, 15, 16, 18,  0, 16, 15, 14,  0, 15, 18, 16,  0, 15,  0, 13,  0, 14, 15, 16,  0, 15, 14, 12,  0, 13, 14, 15,  0, 12,  0,
    15,  0, 18, 19, 20,  0, 19, 18, 16,  0, 15, 14, 15,  0, 18,  0, 16, 18, 19,  0, 18, 16, 15,  0, 14, 15, 16,  0, 15,  0, 12,  0,
    26, 28, 31,  0, 30, 28, 27, 26, 24, 26, 28, 30, 28, 27, 26,  0, 28, 31, 34,  0, 31, 30, 28, 27, 26, 27, 28, 31, 30, 28, 27, 26,
    22, 26, 28, 30, 31, 30, 28, 26, 27, 28, 30, 31, 32, 31, 30, 28, 31, 30, 28, 27, 26, 28, 31, 34, 31, 30, 28, 27, 26,  0, 21,  0,
    26, 28, 30, 31, 32, 31, 30, 28, 27, 28, 30, 31, 30, 28, 27, 26, 28, 30, 31, 34, 31, 30, 28, 27, 26, 27, 28, 30, 31, 30, 28, 27,
    30, 31, 32, 31, 30, 28, 27, 26, 24, 26, 27, 28, 31, 30, 28, 26, 28, 31, 34, 36, 34, 31, 30, 28, 27, 26, 24, 21, 26,  0, 26,  0,
    31, 31, 31, 31, 31, 31, 30, 28, 27, 27, 27, 27, 28, 30, 31,  0, 32, 32, 32, 32, 32, 32, 31, 30, 28, 28, 28, 28, 27, 28, 30,  0,
    31, 31, 31, 31, 28, 28, 28, 28, 26, 26, 26, 26, 27, 28, 30, 31, 30, 30, 30, 30, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26,
    36, 36, 36, 36, 36, 36, 34, 32, 31, 31, 31, 31, 30, 31, 32,  0, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 31, 31, 31, 31,
    28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 27, 28, 30, 31, 32, 32, 32, 32, 31, 31, 30, 28, 29, 29, 29, 29, 36, 36, 36, 36,
    36, 36, 36, 36, 31, 31, 30, 29, 27, 27, 27, 27, 29, 30, 31,  0, 33, 33, 33, 33, 31, 31, 31, 31, 28, 28, 28, 28, 30, 31, 32, 31,
    31, 31, 31, 31, 29, 29, 29, 29, 30, 30, 29, 27, 26, 26, 26, 26, 28, 28, 28, 28, 27, 27, 26, 25, 26, 26, 26, 26, 31, 31, 31, 31,
    36, 36, 36, 36, 35, 33, 31,  0, 30, 30, 30, 30, 29, 30, 31,  0, 32, 32, 32, 32, 31, 31, 31, 31, 30, 30, 29, 27, 26, 26, 26, 26,
    29, 29, 29, 30, 31, 31, 31, 31, 36, 36, 36, 36, 35, 33, 31, 30, 29, 29, 29, 29, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26,
};

static const uint8_t soft7_ch2_seq[BOSS_HOPE_DESPAIR_SOFT6_SAD7_LEN] = {
    12,  0, 12, 13, 15,  0, 14, 12, 11,  0, 12, 14, 13,  0, 12,  0, 10,  0, 11, 12, 13,  0, 12, 11,  9,  0, 10, 11, 12,  0,  9,  0,
    12,  0, 14, 15, 16,  0, 15, 14, 13,  0, 12, 11, 12,  0, 14,  0, 13, 14, 15,  0, 14, 13, 12,  0, 11, 12, 13,  0, 12,  0,  9,  0,
    15, 15,  0, 21, 15, 15,  0, 26, 15, 15,  0, 21, 26, 28, 26, 21, 18, 18,  0, 24, 28,  0, 24, 18, 18, 24, 28,  0, 31, 28, 24,  0,
    13, 13,  0, 18, 13, 13,  0, 22, 13, 13,  0, 18, 22, 26, 22, 18, 14, 14,  0, 19, 14, 14,  0, 24, 14, 14,  0, 19, 24, 27, 24, 19,
    11, 11,  0, 15, 11, 11,  0, 19, 11, 11,  0, 15, 19, 22, 19, 15, 12, 12,  0, 17, 21,  0, 17, 12, 12, 17, 21,  0, 24, 21, 17,  0,
    15, 15,  0, 21, 15, 15,  0, 26, 15, 15,  0, 21, 26, 28, 26, 21, 13, 13,  0, 18, 13, 13,  0, 22, 13, 13,  0, 18, 22, 26, 22, 18,
    28, 28, 28, 28, 28, 28, 27, 26, 24, 24, 24, 24, 26, 27, 28,  0, 30, 30, 30, 30, 30, 30, 28, 27, 26, 26, 26, 26, 24, 26, 27,  0,
    28, 28, 28, 28, 26, 26, 26, 26, 21, 21, 21, 21, 24, 26, 27, 28, 27, 27, 27, 27, 26, 26, 26, 26, 24, 24, 24, 24, 21, 21, 21, 21,
    31, 31, 31, 31, 31, 31, 30, 28, 27, 27, 27, 27, 26, 27, 28,  0, 27, 27, 27, 27, 28, 28, 28, 28, 30, 30, 30, 30, 28, 28, 28, 28,
    26, 26, 26, 26, 24, 24, 24, 24, 21, 21, 21, 21, 24, 26, 27, 28, 30, 30, 30, 30, 28, 28, 27, 26, 21, 21, 21, 21, 29, 29, 29, 29,
    29, 29, 29, 29, 26, 26, 27, 26, 25, 25, 25, 25, 26, 27, 29,  0, 30, 30, 30, 30, 29, 29, 29, 29, 26, 26, 26, 26, 27, 28, 30, 28,
    29, 29, 29, 29, 26, 26, 26, 26, 27, 27, 26, 25, 23, 23, 23, 23, 26, 26, 26, 26, 24, 24, 21, 21, 21, 21, 21, 21, 29, 29, 29, 29,
    31, 31, 31, 31, 30, 29, 27,  0, 26, 26, 26, 26, 27, 29, 30,  0, 30, 30, 30, 30, 29, 29, 29, 29, 27, 27, 26, 25, 21, 21, 21, 21,
    26, 26, 26, 27, 29, 29, 29, 29, 31, 31, 31, 31, 30, 29, 27, 26, 21, 21, 21, 21, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
};

static const uint8_t soft7_bass_seq[BOSS_HOPE_DESPAIR_SOFT6_SAD7_LEN] = {
     2,  0,  2,  5,  2,  0,  5,  2,  2,  0,  5,  2,  5,  0,  2,  0,  6,  0,  6,  3,  6,  0,  3,  6,  6,  0,  3,  6,  3,  0,  6,  0,
     2,  0,  2,  5,  2,  0,  5,  2,  2,  0,  5,  2,  5,  0,  2,  0,  1,  0,  1,  4,  1,  0,  4,  1,  1,  0,  4,  1,  4,  0,  1,  0,
     2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  5,  2,  5,  2,  5,  3,  3,  3,  7,  3,  3,  3,  7,  3,  3,  3,  7,  3,  7,  3,  7,
     6,  6,  6,  3,  6,  6,  6,  3,  6,  6,  6,  3,  6,  3,  6,  3,  1,  1,  1,  4,  1,  1,  1,  4,  1,  1,  1,  4,  1,  4,  1,  4,
     4,  4,  4,  8,  4,  4,  4,  8,  4,  4,  4,  8,  4,  8,  4,  8,  5,  5,  5,  9,  5,  5,  5,  9,  5,  5,  5,  9,  5,  9,  5,  9,
     2,  2,  2,  5,  2,  2,  2,  5,  2,  2,  2,  5,  2,  5,  2,  5,  6,  6,  6,  3,  6,  6,  6,  3,  6,  6,  6,  3,  6,  3,  6,  3,
     8,  0,  0,  0,  8,  0, 12,  0,  8,  0,  0,  0, 12,  0,  8,  0,  6,  0,  0,  0,  6,  0,  3,  0,  6,  0,  0,  0,  3,  0,  6,  0,
     8,  0,  0,  0,  8,  0, 12,  0,  8,  0,  0,  0, 12,  0,  8,  0,  5,  0,  0,  0,  5,  0,  9,  0,  5,  0,  0,  0,  9,  0,  5,  0,
     8,  0,  0,  0,  8,  0, 12,  0,  8,  0,  0,  0, 12,  0,  8,  0,  4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,
     8,  0,  0,  0,  8,  0, 12,  0,  8,  0,  0,  0, 12,  0,  8,  0,  6,  0,  0,  0,  5,  0,  0,  0,  8,  0,  0,  0, 12,  0,  8,  0,
     8,  0,  0,  0, 12,  0,  0,  0,  8,  0,  0,  0, 12,  0,  8,  0,  4,  0,  0,  0,  8,  0,  0,  0,  4,  0,  0,  0,  8,  0,  4,  0,
     8,  0,  0,  0, 12,  0,  0,  0,  8,  0,  0,  0, 12,  0,  8,  0,  6,  0,  0,  0,  3,  0,  0,  0,  6,  0,  0,  0,  3,  0,  6,  0,
     8,  0,  0,  0, 12,  0,  8,  0,  8,  0,  0,  0, 12,  0,  8,  0,  6,  0,  0,  0,  3,  0,  6,  0,  6,  0,  0,  0,  3,  0,  6,  0,
     4,  0,  0,  0,  8,  0,  4,  0,  4,  0,  0,  0,  8,  0,  4,  0,  8,  0,  0,  0, 12,  0,  8,  0,  8,  0,  0,  0, 12,  0,  8,  0,
};


static uint16_t song_pos = 0u;
static uint8_t song_tick = 0u;
static uint8_t song_playing = 0u;
static uint8_t ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;
static uint8_t last_ch2_note = 255u;

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

static void trigger_ch1(uint16_t f, uint8_t part) {
    if (f == 0u) { NR12_REG = 0x00u; return; }
    NR10_REG = 0x00u;
    NR11_REG = (part >= 3u) ? 0x80u : ((part == 0u) ? 0xC0u : 0x80u);
    NR12_REG = (part >= 3u) ? 0x64u : ((part == 0u) ? 0x54u : 0x83u);
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_ch2(uint16_t f, uint8_t part) {
    if (f == 0u) { NR22_REG = 0x00u; return; }
    NR21_REG = (part >= 3u) ? 0xC0u : ((part == 0u) ? 0x80u : 0x40u);
    NR22_REG = (part >= 3u) ? 0x44u : ((part == 0u) ? 0x43u : 0x72u);
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_ch3(uint16_t f, uint8_t retrigger, uint8_t part) {
    if (f == 0u) { NR30_REG = 0x00u; return; }
    if (!ch3_loaded) load_wave();
    NR31_REG = 0xFFu;
    NR32_REG = (part >= 5u) ? 0x40u : 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_drum(uint8_t kind, uint8_t part) {
    NR41_REG = (part == 0u || part >= 3u) ? 0x18u : 0x08u;

    if (kind == 3u) {
        NR42_REG = (part >= 3u) ? 0x21u : 0x31u;
        NR43_REG = (part >= 3u) ? 0x47u : 0x26u;
    } else if (kind == 2u) {
        NR42_REG = (part >= 3u) ? 0x52u : ((part == 0u) ? 0x73u : 0x84u);
        NR43_REG = (part >= 3u) ? 0x45u : ((part == 0u) ? 0x35u : 0x34u);
    } else if (kind == 1u) {
        NR42_REG = (part >= 3u) ? 0x61u : ((part == 0u) ? 0x81u : 0x91u);
        NR43_REG = (part >= 3u) ? 0x56u : ((part == 0u) ? 0x57u : 0x56u);
    } else {
        NR42_REG = 0x21u;
        NR43_REG = 0x77u;
    }
    NR44_REG = 0x80u;
}

static void play_step(uint16_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t part = (uint8_t)(pos / 64u);
    uint8_t lead_note = soft7_lead_seq[pos];
    uint8_t ch2_note = soft7_ch2_seq[pos];

    if ((part >= 3u) && (part != 5u)) {
        if (lead_note != last_lead_note) {
            trigger_ch1(pulse_freq_table[lead_note], part);
            last_lead_note = lead_note;
        }
        if (ch2_note != last_ch2_note) {
            trigger_ch2(pulse_freq_table[ch2_note], part);
            last_ch2_note = ch2_note;
        }
    } else {
        last_lead_note = 255u;
        last_ch2_note = 255u;
        trigger_ch1(pulse_freq_table[lead_note], part);
        trigger_ch2(pulse_freq_table[ch2_note], part);
    }

    set_ch3(wave_freq_table[soft7_bass_seq[pos]], (part > 0u && part < 3u) ? 1u : ((beat == 0u) || (beat == 8u) || (part == 5u)), part);

    if (part == 0u) {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, part);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, part);
        else if ((beat == 7u) || (beat == 15u)) trigger_drum(0u, part);
    } else if (part >= 3u) {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, part);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, part);
        else if ((part == 6u) && ((beat == 6u) || (beat == 14u))) trigger_drum(3u, part);
        else if ((part == 5u) && (beat == 14u)) trigger_drum(0u, part);
    } else {
        if ((beat == 0u) || (beat == 8u)) trigger_drum(1u, part);
        else if ((beat == 4u) || (beat == 12u)) trigger_drum(2u, part);
        else if ((beat == 2u) || (beat == 6u) || (beat == 10u) || (beat == 14u)) trigger_drum(3u, part);
        else trigger_drum(0u, part);
    }
}

void boss_hope_despair_7part_finale_soft6_sad7_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    ch3_loaded = 0u;
    load_wave();

    song_pos = 0u;
    song_tick = 0u;
    song_playing = 1u;
    last_lead_note = 255u;
    last_ch2_note = 255u;

    play_step(0u);
}

void boss_hope_despair_7part_finale_soft6_sad7_bgm_update(void) BANKED {
    if (!song_playing) return;

    ++song_tick;
    if (song_tick >= BOSS_HOPE_DESPAIR_SOFT6_SAD7_STEP_FRAMES) {
        song_tick = 0u;
        ++song_pos;

        if (song_pos >= BOSS_HOPE_DESPAIR_SOFT6_SAD7_LEN) {
            song_pos = 0u;
            last_lead_note = 255u;
            last_ch2_note = 255u;
        }

        play_step(song_pos);
    }
}

void boss_hope_despair_7part_finale_soft6_sad7_bgm_stop(void) BANKED {
    song_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
