#include "audio.h"
#include "deep_eerie_motif_dungeon_bgm.h"
#include "heavy_metal_celtic_battle_bgm.h"
#include "peaceful_harp_town_bgm.h"
#include "sunset_ruins_field_bgm.h"
#include "sunset_strings_adventure_field_bgm.h"

/*
 * rpg144_deep_eerie_dungeon_bgm:
 * Central audio dispatcher.
 *
 * Heavy data-driven BGM players are BANKED and kept out of Bank 0:
 * - FIELD   : sunset_strings_adventure_field_bgm   bank 13
 * - TOWN    : peaceful_harp_town_bgm               bank 12
 * - DUNGEON : deep_eerie_motif_dungeon_bgm       bank 11
 * - RUINS   : sunset_ruins_field_bgm             bank 14
 * - BATTLE  : heavy_metal_celtic_battle_bgm         bank 15
 *
 * audio.c intentionally remains small: it only switches tracks and calls the
 * active BANKED player once per frame.
 */

static UINT8 audio_current_track;
static UINT8 audio_started;

static void audio_stop_external_bgm_if_needed(void) {
    if (audio_current_track == AUDIO_TRACK_FIELD) {
        sunset_strings_adventure_field_bgm_stop();
    } else if (audio_current_track == AUDIO_TRACK_TOWN) {
        peaceful_harp_town_bgm_stop();
    } else if (audio_current_track == AUDIO_TRACK_DUNGEON) {
        deep_eerie_motif_dungeon_bgm_stop();
    } else if (audio_current_track == AUDIO_TRACK_RUINS) {
        sunset_ruins_field_bgm_stop();
    } else if (audio_current_track == AUDIO_TRACK_BATTLE) {
        heavy_metal_celtic_battle_bgm_stop();
    }
}

void audio_init(void) {
    /* Enable master sound, route all channels to both speakers, set modest volume. */
    NR52_REG = 0x80u;
    NR51_REG = 0xFFu;
    NR50_REG = 0x66u;

    audio_current_track = AUDIO_TRACK_NONE;
    audio_started = 1u;
}

void audio_stop_music(void) {
    audio_stop_external_bgm_if_needed();
    audio_current_track = AUDIO_TRACK_NONE;

    NR12_REG = 0x00u;
    NR22_REG = 0x00u;
    NR30_REG = 0x00u;
    NR42_REG = 0x00u;
}

void audio_play_music(UINT8 track) {
    if (!audio_started) audio_init();
    if (audio_current_track == track) return;

    audio_stop_external_bgm_if_needed();
    audio_current_track = track;

    if (track == AUDIO_TRACK_FIELD) {
        sunset_strings_adventure_field_bgm_init();
        return;
    }

    if (track == AUDIO_TRACK_TOWN) {
        peaceful_harp_town_bgm_init();
        return;
    }

    if (track == AUDIO_TRACK_DUNGEON) {
        deep_eerie_motif_dungeon_bgm_init();
        return;
    }

    if (track == AUDIO_TRACK_RUINS) {
        sunset_ruins_field_bgm_init();
        return;
    }

    if (track == AUDIO_TRACK_BATTLE) {
        heavy_metal_celtic_battle_bgm_init();
        return;
    }

    audio_current_track = AUDIO_TRACK_NONE;
}

void audio_update(void) {
    if (audio_current_track == AUDIO_TRACK_FIELD) {
        sunset_strings_adventure_field_bgm_update();
        return;
    }

    if (audio_current_track == AUDIO_TRACK_TOWN) {
        peaceful_harp_town_bgm_update();
        return;
    }

    if (audio_current_track == AUDIO_TRACK_DUNGEON) {
        deep_eerie_motif_dungeon_bgm_update();
        return;
    }

    if (audio_current_track == AUDIO_TRACK_RUINS) {
        sunset_ruins_field_bgm_update();
        return;
    }

    if (audio_current_track == AUDIO_TRACK_BATTLE) {
        heavy_metal_celtic_battle_bgm_update();
        return;
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
