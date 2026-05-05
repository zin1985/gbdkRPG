#include "audio.h"

/*
 * rpg083_music:
 * Tiny register-based DMG music driver.
 *
 * This is intentionally much smaller than adding a tracker driver while the
 * battle renderer is still being stabilized.  It uses square channel 1 for the
 * melody, square channel 2 for a soft bass/drone, and a tiny noise hit for the
 * battle loop.  The melodies are original and only aim for an old handheld RPG
 * mood: field = march, town = calm, battle = urgent.
 */

typedef struct MusicStep {
    UINT16 melody;
    UINT16 bass;
    UINT8 frames;
} MusicStep;

#define N_REST 0u
#define N_C3 1046u
#define N_D3 1155u
#define N_E3 1253u
#define N_F3 1297u
#define N_G3 1379u
#define N_A3 1452u
#define N_B3 1517u
#define N_C4 1546u
#define N_D4 1602u
#define N_E4 1650u
#define N_F4 1673u
#define N_G4 1714u
#define N_A4 1750u
#define N_B4 1781u
#define N_C5 1798u
#define N_D5 1824u
#define N_E5 1847u
#define N_G5 1884u

static const MusicStep music_field[] = {
    {N_C4, N_C3, 12u}, {N_E4, N_C3, 12u}, {N_G4, N_G3, 12u}, {N_C5, N_G3, 12u},
    {N_B4, N_G3, 12u}, {N_G4, N_C3, 12u}, {N_E4, N_C3, 12u}, {N_G4, N_G3, 12u},
    {N_A4, N_F3, 12u}, {N_C5, N_F3, 12u}, {N_A4, N_A3, 12u}, {N_G4, N_G3, 12u},
    {N_E4, N_C3, 12u}, {N_D4, N_G3, 12u}, {N_E4, N_C3, 12u}, {N_REST, N_C3, 12u}
};

static const MusicStep music_town[] = {
    {N_E4, N_C3, 24u}, {N_G4, N_C3, 24u}, {N_A4, N_F3, 24u}, {N_G4, N_F3, 24u},
    {N_E4, N_C3, 24u}, {N_D4, N_G3, 24u}, {N_C4, N_C3, 36u}, {N_REST, N_C3, 12u},
    {N_D4, N_G3, 24u}, {N_E4, N_C3, 24u}, {N_G4, N_E3, 24u}, {N_E4, N_C3, 24u},
    {N_D4, N_G3, 24u}, {N_C4, N_C3, 48u}
};

static const MusicStep music_battle[] = {
    {N_C4, N_C3, 6u}, {N_C4, N_G3, 6u}, {N_G4, N_C3, 6u}, {N_C5, N_G3, 6u},
    {N_D5, N_D3, 6u}, {N_C5, N_G3, 6u}, {N_G4, N_C3, 6u}, {N_E4, N_G3, 6u},
    {N_F4, N_F3, 6u}, {N_A4, N_C3, 6u}, {N_C5, N_F3, 6u}, {N_A4, N_C3, 6u},
    {N_G4, N_G3, 6u}, {N_E4, N_C3, 6u}, {N_D4, N_G3, 6u}, {N_C4, N_C3, 6u}
};

static UINT8 audio_current_track = AUDIO_TRACK_NONE;
static UINT8 audio_step_index = 0u;
static UINT8 audio_step_frames = 0u;
static UINT8 audio_started = 0u;
static UINT8 audio_battle_noise_flip = 0u;

static UINT8 audio_track_length(UINT8 track) {
    if (track == AUDIO_TRACK_FIELD) return (UINT8)(sizeof(music_field) / sizeof(music_field[0]));
    if (track == AUDIO_TRACK_TOWN) return (UINT8)(sizeof(music_town) / sizeof(music_town[0]));
    if (track == AUDIO_TRACK_BATTLE) return (UINT8)(sizeof(music_battle) / sizeof(music_battle[0]));
    return 0u;
}

static const MusicStep *audio_track_step(UINT8 track, UINT8 index) {
    if (track == AUDIO_TRACK_FIELD) return &music_field[index];
    if (track == AUDIO_TRACK_TOWN) return &music_town[index];
    return &music_battle[index];
}

static void audio_trigger_pulse1(UINT16 note) {
    if (note == N_REST) {
        NR12_REG = 0x00u;
        return;
    }

    NR10_REG = 0x00u;
    NR11_REG = 0x80u;
    NR12_REG = 0x74u;
    NR13_REG = (UINT8)(note & 0xFFu);
    NR14_REG = (UINT8)(0x80u | ((note >> 8) & 0x07u));
}

static void audio_trigger_pulse2(UINT16 note) {
    if (note == N_REST) {
        NR22_REG = 0x00u;
        return;
    }

    NR21_REG = 0x40u;
    NR22_REG = 0x42u;
    NR23_REG = (UINT8)(note & 0xFFu);
    NR24_REG = (UINT8)(0x80u | ((note >> 8) & 0x07u));
}

static void audio_trigger_noise_tick(void) {
    NR41_REG = 0x00u;
    NR42_REG = 0x31u;
    NR43_REG = 0x51u;
    NR44_REG = 0x80u;
}

static void audio_trigger_current_step(void) {
    UINT8 len;
    const MusicStep *step;

    len = audio_track_length(audio_current_track);
    if (len == 0u) return;
    if (audio_step_index >= len) audio_step_index = 0u;

    step = audio_track_step(audio_current_track, audio_step_index);
    audio_trigger_pulse1(step->melody);
    audio_trigger_pulse2(step->bass);
    audio_step_frames = step->frames;

    if (audio_current_track == AUDIO_TRACK_BATTLE) {
        audio_battle_noise_flip++;
        if ((audio_battle_noise_flip & 0x03u) == 0u) audio_trigger_noise_tick();
    }

    audio_step_index++;
    if (audio_step_index >= len) audio_step_index = 0u;
}

void audio_init(void) {
    /* Enable master sound, route all channels to both speakers, set modest volume. */
    NR52_REG = 0x80u;
    NR51_REG = 0xFFu;
    NR50_REG = 0x66u;

    audio_current_track = AUDIO_TRACK_NONE;
    audio_step_index = 0u;
    audio_step_frames = 0u;
    audio_started = 1u;
}

void audio_stop_music(void) {
    audio_current_track = AUDIO_TRACK_NONE;
    audio_step_index = 0u;
    audio_step_frames = 0u;
    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR42_REG = 0x00u;
}

void audio_play_music(UINT8 track) {
    if (!audio_started) audio_init();
    if (audio_current_track == track) return;

    audio_current_track = track;
    audio_step_index = 0u;
    audio_step_frames = 0u;
    audio_battle_noise_flip = 0u;
    audio_trigger_current_step();
}

void audio_update(void) {
    if (audio_current_track == AUDIO_TRACK_NONE) return;

    if (audio_step_frames > 0u) {
        audio_step_frames--;
    }
    if (audio_step_frames == 0u) {
        audio_trigger_current_step();
    }
}

void audio_wait_vbl(void) {
    audio_update();
    wait_vbl_done();
}

void audio_wait_frames(UINT8 frames) {
    while (frames > 0u) {
        audio_wait_vbl();
        frames--;
    }
}

void audio_waitpadup(void) {
    while (joypad()) audio_wait_vbl();
}

UINT8 audio_waitpad(UINT8 mask) {
    UINT8 keys;

    while (1) {
        keys = joypad();
        if (keys & mask) return keys;
        audio_wait_vbl();
    }
}
