#pragma bank 25

#include <gb/gb.h>
#include <gb/hardware.h>
#include "cgb_fx_runtime.h"
#include "audio.h"

BANKREF(cgb_fx_runtime_bank)

#define CGB_PAL_AUTOINC 0x80u
#define CGB_RGB5(r, g, b) ((UINT16)((r) | ((UINT16)(g) << 5) | ((UINT16)(b) << 10)))

static const UINT16 cgb_normal_bg_palettes[8u * 4u] = {
    /* 0: neutral/default */
    CGB_RGB5(31,31,28), CGB_RGB5(23,22,18), CGB_RGB5(13,13,12), CGB_RGB5(2,2,2),
    /* 1: PLANT_EYE leaf/body from export 3 */
    CGB_RGB5(31,31,28), CGB_RGB5(16,21,10), CGB_RGB5(4,15,0), CGB_RGB5(1,6,0),
    /* 2: mimic red/yellow */
    CGB_RGB5(31,31,28), CGB_RGB5(30,25,8), CGB_RGB5(24,6,7), CGB_RGB5(8,1,2),
    /* 3: PLANT_EYE iris/accent from export 3 palette 7 */
    CGB_RGB5(31,31,28), CGB_RGB5(28,26,19), CGB_RGB5(24,8,0), CGB_RGB5(9,6,2),
    /* 4: PLANT_GAZE leaf purple from export 4 */
    CGB_RGB5(31,31,28), CGB_RGB5(15,0,23), CGB_RGB5(20,0,24), CGB_RGB5(7,0,9),
    /* 5: PLANT_EYE wood/root from export 3 */
    CGB_RGB5(31,31,28), CGB_RGB5(28,26,19), CGB_RGB5(20,17,10), CGB_RGB5(9,6,2),
    /* 6: PLANT_GAZE wood/root from export 4 */
    CGB_RGB5(31,31,28), CGB_RGB5(31,29,25), CGB_RGB5(22,15,0), CGB_RGB5(13,6,0),
    /* 7: PLANT_GAZE iris/green accent from export 4 */
    CGB_RGB5(31,31,28), CGB_RGB5(28,26,19), CGB_RGB5(0,22,1), CGB_RGB5(9,6,2)
};

static const UINT16 cgb_normal_obj_palettes[8u * 4u] = {
    /* 0: skeleton neutral gray */
    CGB_RGB5(31,31,31), CGB_RGB5(24,23,21), CGB_RGB5(12,11,10), CGB_RGB5(0,0,0),
    /* 1: skeleton red cape */
    CGB_RGB5(31,31,31), CGB_RGB5(29,14,14), CGB_RGB5(19,5,6), CGB_RGB5(7,0,1),
    /* 2: PLANT_GAZE leaf purple from export 4 */
    CGB_RGB5(31,31,31), CGB_RGB5(16,0,24), CGB_RGB5(20,0,24), CGB_RGB5(6,0,8),
    /* 3: PLANT_EYE wood/root from export 3 */
    CGB_RGB5(31,31,31), CGB_RGB5(28,26,19), CGB_RGB5(18,13,7), CGB_RGB5(8,5,2),
    /* 4: PLANT_EYE leaf green from export 3 */
    CGB_RGB5(31,31,31), CGB_RGB5(15,23,9), CGB_RGB5(4,15,0), CGB_RGB5(1,7,1),
    /* 5: PLANT_GAZE wood/root from export 4 */
    CGB_RGB5(31,31,31), CGB_RGB5(28,26,19), CGB_RGB5(18,13,7), CGB_RGB5(8,5,2),
    /* 6: spare enemy OBJ palette */
    CGB_RGB5(31,31,31), CGB_RGB5(24,23,21), CGB_RGB5(12,11,10), CGB_RGB5(0,0,0),
    /* 7: spare enemy OBJ palette */
    CGB_RGB5(31,31,31), CGB_RGB5(24,23,21), CGB_RGB5(12,11,10), CGB_RGB5(0,0,0)
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

static void cgb_restore_obj_palettes(void) {
    UINT8 i;
    UINT16 c;
    OCPS_REG = CGB_PAL_AUTOINC;
    for (i = 0u; i < 32u; i++) {
        c = cgb_normal_obj_palettes[i];
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
    cgb_restore_obj_palettes();
}
