#ifndef AUDIO_H
#define AUDIO_H

#include <gb/gb.h>

#define AUDIO_TRACK_NONE   0u
#define AUDIO_TRACK_FIELD  1u
#define AUDIO_TRACK_TOWN   2u
#define AUDIO_TRACK_BATTLE 3u

void audio_init(void);
void audio_play_music(UINT8 track);
void audio_stop_music(void);
void audio_update(void);
void audio_wait_vbl(void);
void audio_wait_frames(UINT8 frames);
void audio_waitpadup(void);
UINT8 audio_waitpad(UINT8 mask);

#endif
