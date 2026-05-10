#include <gb/gb.h>
#include "audio.h"
#include "revive_runtime.h"

#pragma bank 5

UINT8 revive_runtime_wait_choice_ab(void) BANKED {
    UINT8 keys;
    audio_waitpadup();
    while (1) {
        keys = joypad();
        if (keys & J_A) {
            audio_waitpadup();
            return 1u;
        }
        if (keys & J_B) {
            audio_waitpadup();
            return 0u;
        }
        audio_wait_vbl();
    }
}
