#pragma bank 8

#include <gb/gb.h>
#include "ui_icons.h"
#include "inventory.h"
#include "party_runtime.h"
#include "jpfont.h"

/* rpg181: user-provided 8x8 category icons.
 * Order: item, sword, staff, tool-weapon, bow, armor, cloth, accessory.
 * Loaded into BG tile IDs 244-251, outside JP font cache (128-223),
 * frame tiles (224-232), and field minimap tiles (240-243).
 */
static const unsigned char ui_icon_tiles[8u * 16u] = {
    0x7E,0x7E,0x7E,0x42,0x3C,0x24,0x24,0x3C,0x52,0x6E,0xA1,0xDF,0x81,0xFF,0x7E,0x7E,
    0x03,0x03,0x05,0x05,0x0A,0x0A,0xD4,0xD4,0xA8,0xE8,0x50,0x70,0xE8,0xF8,0xD8,0xD8,
    0x0E,0x0E,0x31,0x1F,0x6D,0x3F,0x59,0x7F,0xDF,0x77,0xA7,0xE0,0xA0,0xE0,0xC0,0xC0,
    0x02,0x02,0x37,0x07,0xFA,0xFA,0xFA,0x8A,0xD8,0x50,0xCB,0xBA,0x8F,0xFD,0xFB,0xFA,
    0xE7,0xE7,0x9B,0xBB,0x65,0x5D,0x2A,0x2E,0x12,0x16,0x29,0x2B,0x45,0x45,0x83,0x83,
    0x66,0x66,0x99,0xFF,0xA5,0xFF,0x42,0x7E,0x3C,0x3C,0x5A,0x7E,0x66,0x7E,0x18,0x18,
    0x3E,0x24,0x7F,0x5A,0xFF,0x81,0xFF,0xA5,0xFF,0xE7,0x7E,0x42,0xFE,0x42,0xFE,0x7E,
    0x0A,0x0E,0x20,0x31,0x21,0x41,0x40,0x41,0x71,0x73,0x94,0xBC,0x90,0xF0,0xE0,0xE0
};

void ui_icons_load(void) BANKED {
    set_bkg_data(UI_ICON_TILE_BASE, 8u, ui_icon_tiles);
}

UINT8 ui_icon_tile(UINT8 icon_id) BANKED {
    if (icon_id >= 8u) return (UINT8)(JP_FRAME_BASE + 0u);
    return (UINT8)(UI_ICON_TILE_BASE + icon_id);
}

UINT8 ui_icon_tile_for_item(UINT8 item_id) BANKED {
    switch (item_id) {
        case ITEM_WOOD_SWORD:
        case ITEM_IRON_SWORD:
            return ui_icon_tile(UI_ICON_SWORD);
        case ITEM_OAK_STAFF:
        case ITEM_MAGE_STAFF:
            return ui_icon_tile(UI_ICON_STAFF);
        case ITEM_SHORT_BOW:
            return ui_icon_tile(UI_ICON_BOW);
        case ITEM_GLOVES:
        case ITEM_TOOL_KIT:
            return ui_icon_tile(UI_ICON_TOOL_WEAPON);
        case ITEM_CLOTH_ARMOR:
            return ui_icon_tile(UI_ICON_CLOTH);
        case ITEM_LEATHER_ARMOR:
        case ITEM_IRON_ARMOR:
            return ui_icon_tile(UI_ICON_ARMOR);
        case ITEM_CHARM:
        case ITEM_FEATHER:
        case ITEM_DEBUG_NO_ENCOUNT:
        case ITEM_DEBUG_ESCAPE:
            return ui_icon_tile(UI_ICON_ACCESSORY);
        default:
            return ui_icon_tile(UI_ICON_ITEM);
    }
}

UINT8 ui_icon_tile_for_weapon_type(UINT8 weapon_type) BANKED {
    switch (weapon_type) {
        case PARTY_WEAPON_SWORD: return ui_icon_tile(UI_ICON_SWORD);
        case PARTY_WEAPON_STAFF: return ui_icon_tile(UI_ICON_STAFF);
        case PARTY_WEAPON_BOW:   return ui_icon_tile(UI_ICON_BOW);
        case PARTY_WEAPON_GLOVE: return ui_icon_tile(UI_ICON_TOOL_WEAPON);
        case PARTY_WEAPON_TOOL:  return ui_icon_tile(UI_ICON_TOOL_WEAPON);
        default: return (UINT8)(JP_FRAME_BASE + 0u);
    }
}

void ui_put_icon(UINT8 x, UINT8 y, UINT8 icon_tile) BANKED {
    set_bkg_tiles(x, y, 1u, 1u, &icon_tile);
}
