#pragma bank 11
#include "deep_eerie_motif_dungeon_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define DEEP_EERIE_DUNGEON_LEN 256u
#define DEEP_EERIE_DUNGEON_STEP_FRAMES 14u

/*
 * Original deep eerie dungeon BGM for Game Boy / GBDK.
 *
 * Arrangement concept:
 * - Takes the stronger latter-half phrase from the previous dungeon draft.
 * - Lowers it and makes it more suspicious.
 * - Keeps the field motif only as a buried memory.
 *
 * Channels:
 * - CH1: low haunted motif
 * - CH2: sparse cold echo / harp drop
 * - CH3: heavier cave drone
 * - CH4: cave breath / distant scrape
 */

static const uint16_t pulse_freq_table[] = {
       0u,  458u,  547u,  711u,  856u,  986u, 1046u, 1155u, 1253u, 1297u,
    1379u, 1452u, 1517u, 1547u, 1602u, 1650u, 1673u, 1714u, 1750u, 1783u,
    1798u, 1825u, 1849u, 1860u, 1881u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1253u, 1297u, 1379u, 1452u, 1517u, 1547u, 1602u, 1650u, 1673u,
    1714u, 1750u, 1783u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u, 1915u,
    1923u, 1936u, 1949u, 1954u, 1964u,
};

static const uint8_t deep_dungeon_lead_seq[DEEP_EERIE_DUNGEON_LEN] = {
    11, 11, 11,  0, 13, 13,  0, 14, 16, 16,  0, 15,  0, 13,  0, 11, 10, 10,  0, 11, 13, 13,  0, 14, 13, 13,  0, 11, 10,  0,  8,  0,
     9,  9,  9,  9, 10, 10,  0, 11, 13, 13, 13,  0, 14,  0, 15,  0, 14, 14,  0, 13, 11, 11,  0, 10, 11, 11, 11, 11,  0,  0,  0,  0,
     4,  4,  4,  4,  6,  6,  0,  7,  9,  9,  0,  8,  0,  6,  0,  4,  3,  3,  0,  4,  6,  6,  0,  7,  6,  6,  0,  4,  3,  0,  1,  0,
     2,  2,  2,  2,  3,  3,  0,  4,  6,  6,  6,  0,  7,  0,  8,  0,  7,  7,  0,  6,  4,  4,  0,  3,  4,  4,  4,  4,  0,  0,  0,  0,
    11, 11,  0, 13, 12, 12,  0, 13, 14, 14, 14,  0, 16, 15,  0, 13, 10, 10,  0, 12, 13, 13,  0, 11,  9,  9,  9,  0,  8,  0, 10,  0,
     4,  4,  4,  4,  8,  8,  0,  9, 10, 10,  0, 11, 13,  0, 12,  0, 13, 13,  0, 12, 11, 11, 11, 11, 10, 10,  9,  9,  8,  8,  8,  8,
    11, 11, 11,  0, 13, 13,  0, 14, 16, 16,  0, 15, 14,  0, 13,  0,  4,  4,  4,  4,  0,  0,  6,  6,  7,  7,  7,  0,  9,  8,  0,  6,
     2,  2,  2,  2,  3,  3,  0,  4,  5,  5,  5,  0,  6,  0,  7,  0,  4,  4,  4,  4,  4,  4,  0,  0,  3,  3,  3,  3,  4,  4,  4,  4,
};

static const uint8_t deep_dungeon_echo_seq[DEEP_EERIE_DUNGEON_LEN] = {
    18,  0,  0,  0, 15,  0,  0,  0, 13,  0,  0,  0, 14,  0,  0,  0, 17,  0,  0,  0, 14,  0,  0,  0, 11,  0,  0,  0, 15,  0,  0,  0,
    16,  0,  0,  0, 13,  0,  0,  0, 11,  0,  0,  0, 15,  0,  0,  0, 14,  0,  0,  0, 11,  0,  0,  0, 10,  0,  0,  0, 11,  0,  0,  0,
    18,  0, 15,  0, 13,  0,  0,  0, 14,  0, 16,  0, 15,  0,  0,  0, 17,  0, 14,  0, 11,  0,  0,  0, 13,  0, 11,  0,  8,  0,  0,  0,
    16,  0, 13,  0, 10,  0,  0,  0, 11,  0, 13,  0, 14,  0,  0,  0, 14,  0, 11,  0, 13,  0,  0,  0, 11,  0, 10,  0, 11,  0,  0,  0,
    18,  0,  0, 20, 19,  0,  0, 20, 21,  0,  0, 23, 22,  0, 20,  0, 17,  0,  0, 19, 20,  0,  0, 18, 16,  0,  0, 15, 17,  0,  0,  0,
    18,  0, 15,  0, 16,  0, 17,  0, 18,  0, 20,  0, 19,  0,  0,  0, 20,  0, 19,  0, 18,  0, 17,  0, 16,  0, 15,  0,  0,  0,  0,  0,
    18,  0,  0,  0, 15,  0,  0,  0, 13,  0,  0, 14, 16,  0, 15,  0, 11,  0,  0,  0, 13,  0,  0,  0, 14,  0,  0,  0, 16,  0, 15,  0,
    16,  0,  0,  0, 17,  0,  0,  0, 19,  0,  0,  0, 20,  0, 21,  0, 11,  0,  0,  0,  8,  0,  0,  0, 10,  0,  0,  0, 11,  0,  0,  0,
};

static const uint8_t deep_dungeon_bass_seq[DEEP_EERIE_DUNGEON_LEN] = {
     4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,  3,  0,  0,  0,  3,  0,  7,  0,  3,  0,  0,  0,  7,  0,  3,  0,
     2,  0,  0,  0,  2,  0,  6,  0,  2,  0,  0,  0,  6,  0,  2,  0,  4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,
     4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,  3,  0,  0,  0,  3,  0,  7,  0,  3,  0,  0,  0,  7,  0,  3,  0,
     2,  0,  0,  0,  2,  0,  6,  0,  2,  0,  0,  0,  6,  0,  2,  0,  4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,
     4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,  3,  0,  0,  0,  3,  0,  7,  0,  3,  0,  0,  0,  7,  0,  3,  0,
     4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,  1,  0,  0,  0,  1,  0,  5,  0,  1,  0,  0,  0,  5,  0,  1,  0,
     4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,  4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,
     2,  0,  0,  0,  2,  0,  6,  0,  2,  0,  0,  0,  6,  0,  2,  0,  4,  0,  0,  0,  4,  0,  8,  0,  4,  0,  0,  0,  8,  0,  4,  0,
};


static uint16_t deep_pos = 0u;
static uint8_t deep_tick = 0u;
static uint8_t deep_playing = 0u;
static uint8_t deep_ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;

static void load_deep_wave(void) {
    static const uint8_t wave_data[16] = {
        0x9Bu,0xBDu,0xDEu,0xFEu,0xECu,0xA7u,0x52u,0x20u,
        0x02u,0x25u,0x7Au,0xBEu,0xFDu,0xDBu,0x86u,0x43u
    };

    uint8_t i;
    NR30_REG = 0x00u;
    for (i = 0u; i != 16u; ++i) {
        *((uint8_t *)0xFF30u + i) = wave_data[i];
    }
    NR30_REG = 0x80u;
    deep_ch3_loaded = 1u;
}

static void trigger_low_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0xC0u; /* rounder, darker pulse */
    NR12_REG = 0x44u; /* quiet slow decay */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_echo_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0x00u; /* thin pluck */
    NR22_REG = 0x32u; /* colder and quieter */
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_deep_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!deep_ch3_loaded) {
        load_deep_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x40u; /* louder wave channel */
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_deep_noise(uint8_t kind) {
    NR41_REG = 0x30u;

    if (kind == 1u) {
        NR42_REG = 0x32u;
        NR43_REG = 0x65u;
    } else {
        NR42_REG = 0x11u;
        NR43_REG = 0x76u;
    }

    NR44_REG = 0x80u;
}

static void deep_dungeon_play_step(uint16_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t lead_note = deep_dungeon_lead_seq[pos];

    /* Low lead sustains repeated notes. */
    if (lead_note != last_lead_note) {
        trigger_low_lead_ch1(pulse_freq_table[lead_note]);
        last_lead_note = lead_note;
    }

    trigger_echo_ch2(pulse_freq_table[deep_dungeon_echo_seq[pos]]);

    set_deep_bass_ch3(wave_freq_table[deep_dungeon_bass_seq[pos]], ((beat & 7u) == 0u));

    if (beat == 0u) {
        trigger_deep_noise(1u);
    } else if ((beat == 5u) || (beat == 11u) || (beat == 14u)) {
        trigger_deep_noise(0u);
    }
}

void deep_eerie_motif_dungeon_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x66u;
    NR51_REG = 0xFFu;

    deep_ch3_loaded = 0u;
    load_deep_wave();

    deep_pos = 0u;
    deep_tick = 0u;
    deep_playing = 1u;
    last_lead_note = 255u;

    deep_dungeon_play_step(0u);
}

void deep_eerie_motif_dungeon_bgm_update(void) BANKED {
    if (!deep_playing) return;

    ++deep_tick;
    if (deep_tick >= DEEP_EERIE_DUNGEON_STEP_FRAMES) {
        deep_tick = 0u;
        ++deep_pos;

        if (deep_pos >= DEEP_EERIE_DUNGEON_LEN) {
            deep_pos = 0u;
            last_lead_note = 255u;
        }

        deep_dungeon_play_step(deep_pos);
    }
}

void deep_eerie_motif_dungeon_bgm_stop(void) BANKED {
    deep_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
