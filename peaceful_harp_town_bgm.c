#pragma bank 12
#include "peaceful_harp_town_bgm.h"

#include <gb/gb.h>
#include <stdint.h>

#define PEACEFUL_TOWN_LEN 384u
#define PEACEFUL_TOWN_STEP_FRAMES 10u

/*
 * Original peaceful harp town BGM for Game Boy / GBDK.
 *
 * Mood:
 * - calm town, safe inn, warm lamps, gentle night street
 * - inspired by the feeling of classic RPG town themes, but with original melody
 * - CH2 imitates harp-like plucks with short bright arpeggios
 *
 * Channels:
 * - CH1: soft sustained melody
 * - CH2: harp-like arpeggio
 * - CH3: warm bass/pad
 * - CH4: very soft room/air noise
 */

static const uint16_t pulse_freq_table[] = {
       0u,  547u,  711u,  856u, 1046u, 1155u, 1253u, 1379u, 1547u, 1650u,
    1673u, 1714u, 1750u, 1798u, 1825u, 1849u, 1860u, 1881u, 1899u,
};

static const uint16_t wave_freq_table[] = {
       0u, 1297u, 1379u, 1452u, 1547u, 1602u, 1650u, 1714u, 1798u, 1849u,
    1860u, 1881u, 1899u, 1923u, 1936u, 1949u, 1954u, 1964u, 1974u,
};

static const uint8_t town_lead_seq[PEACEFUL_TOWN_LEN] = {
    11, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 13, 13, 12, 12, 11, 11, 11, 11,  9,  9, 11, 11, 12, 12, 12, 12, 11, 11,  9,  9,
    10, 10, 10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 12, 12, 11, 11,  9,  9,  9,  9, 11, 11, 12, 12, 11, 11, 11, 11,  9,  9,  8,  8,
    11, 11, 11, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 14, 14, 13, 13, 13, 13, 12, 12, 11, 11,  9,  9,  9,  9, 11, 11, 12, 12,
    13, 13, 13, 13, 14, 14, 15, 15, 17, 17, 17, 17, 15, 15, 14, 14, 13, 13, 13, 13, 12, 12, 11, 11, 11, 11, 11, 11,  0,  0,  0,  0,
    15, 15, 15, 15, 14, 14, 13, 13, 12, 12, 12, 12, 11, 11, 12, 12, 13, 13, 13, 13, 14, 14, 15, 15, 14, 14, 14, 14, 13, 13, 12, 12,
    11, 11, 11, 11, 12, 12, 13, 13, 15, 15, 15, 15, 14, 14, 13, 13, 12, 12, 12, 12, 11, 11,  9,  9, 11, 11, 11, 11,  0,  0,  0,  0,
    13, 13, 13, 13, 14, 14, 15, 15, 17, 17, 17, 17, 15, 15, 14, 14, 13, 13, 13, 13, 12, 12, 11, 11, 12, 12, 12, 12, 13, 13, 14, 14,
    15, 15, 15, 15, 17, 17, 18, 18, 17, 17, 17, 17, 15, 15, 14, 14, 13, 13, 13, 13, 12, 12, 11, 11,  9,  9,  9,  9,  0,  0,  0,  0,
    11, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 14, 13, 13, 12, 12, 11, 11, 11, 11,  9,  9, 11, 11, 12, 12, 12, 12, 11, 11,  9,  9,
    10, 10, 10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 12, 12, 11, 11,  9,  9,  9,  9, 11, 11, 12, 12, 11, 11, 11, 11,  9,  9,  8,  8,
    13, 13, 13, 13, 14, 14, 15, 15, 14, 14, 14, 14, 13, 13, 12, 12, 11, 11, 11, 11, 12, 12, 13, 13, 12, 12, 12, 12, 11, 11,  9,  9,
    10, 10, 10, 10, 11, 11, 12, 12, 13, 13, 13, 13, 14, 14, 13, 13, 12, 12, 12, 12, 11, 11,  9,  9,  8,  8,  8,  8,  8,  8,  8,  8,
};

static const uint8_t town_harp_seq[PEACEFUL_TOWN_LEN] = {
    13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0, 11,  0, 14,  0, 17,  0, 14,  0, 11,  0, 14,  0, 14,  0, 14,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0,
    13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0, 12,  0, 13,  0, 15,  0, 13,  0, 12,  0, 13,  0, 13,  0, 13,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 11,  0, 14,  0, 17,  0, 14,  0, 11,  0, 14,  0, 14,  0, 14,  0,
    12,  0, 13,  0, 15,  0, 13,  0, 12,  0, 13,  0, 13,  0, 13,  0, 10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0,
    13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0, 11,  0, 14,  0, 17,  0, 14,  0, 11,  0, 14,  0, 14,  0, 14,  0,
    13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0, 12,  0, 13,  0, 15,  0, 13,  0, 12,  0, 13,  0, 13,  0, 13,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 11,  0, 14,  0, 17,  0, 14,  0, 11,  0, 14,  0, 14,  0, 14,  0,
    13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0, 11,  0, 14,  0, 17,  0, 14,  0, 11,  0, 14,  0, 14,  0, 14,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0,
    10,  0, 13,  0, 18,  0, 13,  0, 10,  0, 13,  0, 13,  0, 13,  0, 13,  0, 15,  0, 17,  0, 15,  0, 13,  0, 15,  0, 15,  0, 15,  0,
};

static const uint8_t town_bass_seq[PEACEFUL_TOWN_LEN] = {
     4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,  2,  0,  0,  0,  5,  0,  0,  0,  2,  0,  0,  0,  5,  0,  2,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,
     4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,  3,  0,  0,  0,  6,  0,  0,  0,  3,  0,  0,  0,  6,  0,  3,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  2,  0,  0,  0,  5,  0,  0,  0,  2,  0,  0,  0,  5,  0,  2,  0,
     3,  0,  0,  0,  6,  0,  0,  0,  3,  0,  0,  0,  6,  0,  3,  0,  1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,
     4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,  2,  0,  0,  0,  5,  0,  0,  0,  2,  0,  0,  0,  5,  0,  2,  0,
     4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,  3,  0,  0,  0,  6,  0,  0,  0,  3,  0,  0,  0,  6,  0,  3,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  2,  0,  0,  0,  5,  0,  0,  0,  2,  0,  0,  0,  5,  0,  2,  0,
     4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,  2,  0,  0,  0,  5,  0,  0,  0,  2,  0,  0,  0,  5,  0,  2,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,
     1,  0,  0,  0,  4,  0,  0,  0,  1,  0,  0,  0,  4,  0,  1,  0,  4,  0,  0,  0,  7,  0,  0,  0,  4,  0,  0,  0,  7,  0,  4,  0,
};


static uint16_t town_pos = 0u;
static uint8_t town_tick = 0u;
static uint8_t town_playing = 0u;
static uint8_t town_ch3_loaded = 0u;
static uint8_t last_lead_note = 255u;

static void load_town_wave(void) {
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
    town_ch3_loaded = 1u;
}

static void trigger_lead_ch1(uint16_t f) {
    if (f == 0u) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0xC0u; /* softer 75% duty */
    NR12_REG = 0x65u; /* gentle sustained envelope */
    NR13_REG = (uint8_t)(f & 0xFFu);
    NR14_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void trigger_harp_ch2(uint16_t f) {
    if (f == 0u) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0x00u; /* 12.5% duty, harp-ish thin pluck */
    NR22_REG = 0x62u; /* short decay */
    NR23_REG = (uint8_t)(f & 0xFFu);
    NR24_REG = (uint8_t)(0x80u | ((f >> 8u) & 0x07u));
}

static void set_bass_ch3(uint16_t f, uint8_t retrigger) {
    if (f == 0u) {
        NR30_REG = 0x00u;
        return;
    }

    if (!town_ch3_loaded) {
        load_town_wave();
    }

    NR31_REG = 0xFFu;
    NR32_REG = 0x60u;
    NR33_REG = (uint8_t)(f & 0xFFu);
    NR34_REG = (uint8_t)((retrigger ? 0x80u : 0x00u) | ((f >> 8u) & 0x07u));
}

static void trigger_soft_room_noise(uint8_t kind) {
    NR41_REG = 0x20u;

    if (kind == 1u) {
        NR42_REG = 0x21u;
        NR43_REG = 0x67u;
    } else {
        NR42_REG = 0x11u;
        NR43_REG = 0x76u;
    }

    NR44_REG = 0x80u;
}

static void town_play_step(uint16_t pos) {
    uint8_t beat = (uint8_t)(pos & 15u);
    uint8_t lead_note = town_lead_seq[pos];

    /* Lead is sustained when the same note continues. */
    if (lead_note != last_lead_note) {
        trigger_lead_ch1(pulse_freq_table[lead_note]);
        last_lead_note = lead_note;
    }

    /* Harp should be plucked every note, so always retrigger CH2. */
    trigger_harp_ch2(pulse_freq_table[town_harp_seq[pos]]);

    /* Bass stays broad and calm. */
    set_bass_ch3(wave_freq_table[town_bass_seq[pos]], ((beat & 7u) == 0u));

    if ((beat == 0u) || (beat == 8u)) {
        trigger_soft_room_noise(1u);
    } else if ((beat == 6u) || (beat == 14u)) {
        trigger_soft_room_noise(0u);
    }
}

void peaceful_harp_town_bgm_init(void) BANKED {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;

    town_ch3_loaded = 0u;
    load_town_wave();

    town_pos = 0u;
    town_tick = 0u;
    town_playing = 1u;
    last_lead_note = 255u;

    town_play_step(0u);
}

void peaceful_harp_town_bgm_update(void) BANKED {
    if (!town_playing) return;

    ++town_tick;
    if (town_tick >= PEACEFUL_TOWN_STEP_FRAMES) {
        town_tick = 0u;
        ++town_pos;

        if (town_pos >= PEACEFUL_TOWN_LEN) {
            town_pos = 0u;
            last_lead_note = 255u;
        }

        town_play_step(town_pos);
    }
}

void peaceful_harp_town_bgm_stop(void) BANKED {
    town_playing = 0u;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}
