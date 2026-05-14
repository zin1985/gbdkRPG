#pragma bank 25

#include <gb/gb.h>
#include <gb/hardware.h>
#include "cgb_fx_runtime.h"
#include "audio.h"

BANKREF(cgb_fx_runtime_bank)

#define CGB_PAL_AUTOINC 0x80u
#define CGB_RGB5(r, g, b) ((UINT16)((r) | ((UINT16)(g) << 5) | ((UINT16)(b) << 10)))

static const UINT16 cgb_normal_bg_palettes[8u * 4u] = {
    CGB_RGB5(31,31,28), CGB_RGB5(23,22,18), CGB_RGB5(13,13,12), CGB_RGB5(2,2,2),
    CGB_RGB5(30,31,24), CGB_RGB5(18,27,13), CGB_RGB5(8,18,7),  CGB_RGB5(2,5,2),
    CGB_RGB5(30,29,24), CGB_RGB5(22,19,14), CGB_RGB5(13,11,9), CGB_RGB5(3,3,3),
    CGB_RGB5(25,30,31), CGB_RGB5(11,21,30), CGB_RGB5(5,10,20), CGB_RGB5(1,2,6),
    CGB_RGB5(31,29,20), CGB_RGB5(29,19,9),  CGB_RGB5(18,8,5),  CGB_RGB5(4,2,1),
    CGB_RGB5(28,31,22), CGB_RGB5(12,24,9),  CGB_RGB5(4,13,4),  CGB_RGB5(1,4,1),
    CGB_RGB5(31,30,18), CGB_RGB5(30,22,5),  CGB_RGB5(18,10,2), CGB_RGB5(4,2,0),
    CGB_RGB5(24,24,27), CGB_RGB5(14,14,18), CGB_RGB5(7,7,11),  CGB_RGB5(1,1,3)
};

static const UINT16 cgb_normal_obj_palette0[4u] = {
    CGB_RGB5(31,31,31), CGB_RGB5(23,23,20), CGB_RGB5(11,10,9), CGB_RGB5(0,0,0)
};

static void cgb_write_all_bg(UINT16 color) {
    UINT8 i;
    BCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 32u; i++) {
        BCPD_REG = (UINT8)(color & 0xFFu);
        BCPD_REG = (UINT8)(color >> 8u);
    }
}

static void cgb_restore_bg_palettes(void) {
    UINT8 i;
    UINT16 c;
    BCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 32u; i++) {
        c = cgb_normal_bg_palettes[i];
        BCPD_REG = (UINT8)(c & 0xFFu);
        BCPD_REG = (UINT8)(c >> 8u);
    }
}

static void cgb_restore_obj_palette0(void) {
    UINT8 i;
    UINT16 c;
    OCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 4u; i++) {
        c = cgb_normal_obj_palette0[i];
        OCPD_REG = (UINT8)(c & 0xFFu);
        OCPD_REG = (UINT8)(c >> 8u);
    }
}

void cgb_fx_battle_start_flash(void) BANKED {
    UINT8 i;
    for (i = 0u; i < 3u; i++) {
        cgb_write_all_bg(CGB_RGB5(31,31,31));
        audio_wait_vbl();
        audio_wait_vbl();
        cgb_write_all_bg(CGB_RGB5(3,3,3));
        audio_wait_vbl();
        audio_wait_vbl();
    }
    cgb_restore_bg_palettes();
    cgb_restore_obj_palette0();
}
