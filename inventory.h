#ifndef INVENTORY_H
#define INVENTORY_H

#include <gb/gb.h>

#define INVENTORY_ITEM_MAX 128u
#define INVENTORY_STACK_MAX 99u

#define ITEM_NONE          0u
#define ITEM_HERB          1u
#define ITEM_POTION        2u
#define ITEM_CASTLE_KEY    3u
#define ITEM_MANA_HERB     4u
#define ITEM_ANTIDOTE      5u

#define ITEM_WOOD_SWORD    10u
#define ITEM_IRON_SWORD    11u
#define ITEM_OAK_STAFF     12u
#define ITEM_MAGE_STAFF    13u
#define ITEM_SHORT_BOW     14u
#define ITEM_GLOVES        15u
#define ITEM_TOOL_KIT      16u

#define ITEM_CLOTH_ARMOR   24u
#define ITEM_LEATHER_ARMOR 25u
#define ITEM_IRON_ARMOR    26u

#define ITEM_CHARM         32u
#define ITEM_FEATHER       33u
#define ITEM_DEBUG_NO_ENCOUNT 34u
#define ITEM_DEBUG_ESCAPE 35u

/* rpg182: expanded usable items.  Keep IDs below INVENTORY_ITEM_MAX. */
#define ITEM_MEDICINE      40u
#define ITEM_HIGH_POTION   41u
#define ITEM_ELIXIR        42u
#define ITEM_MANA_DROP     43u
#define ITEM_MANA_BOTTLE   44u
#define ITEM_FULL_HERB     45u
#define ITEM_PANACEA       46u
#define ITEM_WAKE_HERB     47u
#define ITEM_EYE_DROP      48u
#define ITEM_SMOKE_BOMB    49u
#define ITEM_BARRIER_SEED  50u
#define ITEM_POWER_SEED    51u
#define ITEM_GUARD_SEED    52u
#define ITEM_SPEED_SEED    53u
#define ITEM_FOCUS_TEA     54u
#define ITEM_MORALE_MEAT   55u
#define ITEM_ICE_BOMB      56u
#define ITEM_FIRE_BOMB     57u
#define ITEM_THUNDER_BOMB  58u
#define ITEM_LIGHT_BOTTLE  59u
#define ITEM_DARK_BOTTLE   60u
#define ITEM_RETURN_FEATHER 61u
#define ITEM_TENT          62u
#define ITEM_REVIVE_STONE  63u

/* rpg182: expanded equipment items. */
#define ITEM_COPPER_SWORD  64u
#define ITEM_STEEL_SWORD   65u
#define ITEM_SILVER_SWORD  66u
#define ITEM_FLAME_SWORD   67u
#define ITEM_ICE_SWORD     68u
#define ITEM_DRAGON_SWORD  69u
#define ITEM_WILLOW_STAFF  70u
#define ITEM_SAGE_STAFF    71u
#define ITEM_THUNDER_STAFF 72u
#define ITEM_MOON_STAFF    73u
#define ITEM_SHADOW_STAFF  74u
#define ITEM_LONG_BOW      75u
#define ITEM_STEEL_BOW     76u
#define ITEM_HUNTER_BOW    77u
#define ITEM_THUNDER_BOW   78u
#define ITEM_STAR_BOW      79u
#define ITEM_IRON_GLOVES   80u
#define ITEM_BEAST_CLAWS   81u
#define ITEM_POWER_KNUCKLE 82u
#define ITEM_MONK_FIST     83u
#define ITEM_ALCHEMY_KIT   84u
#define ITEM_TRAP_TOOL     85u
#define ITEM_BOMB_TOOL     86u
#define ITEM_GEAR_TOOL     87u
#define ITEM_TRAVEL_CLOTH  88u
#define ITEM_MAGE_ROBE     89u
#define ITEM_BATTLE_CLOTH  90u
#define ITEM_CHAIN_MAIL    91u
#define ITEM_STEEL_ARMOR   92u
#define ITEM_SILVER_ARMOR  93u
#define ITEM_DRAGON_ARMOR  94u
#define ITEM_QUICK_BOOTS   95u
#define ITEM_POWER_RING    96u
#define ITEM_GUARD_RING    97u
#define ITEM_FOCUS_RING    98u
#define ITEM_LUCKY_COIN    99u
#define ITEM_DRAGON_SCALE  100u
#define ITEM_SLEEP_GUARD   101u
#define ITEM_POISON_GUARD  102u
#define ITEM_LIGHT_MEDAL   103u
#define ITEM_DARK_MEDAL    104u
#define ITEM_ITIL_SWORD   105u
#define ITEM_ITIL_ARMOR   106u
#define ITEM_ITIL_CREST   107u

void inventory_clear(void) BANKED;
void inventory_seed_defaults(void) BANKED;
UINT8 inventory_get_count(UINT8 item_id) BANKED;
UINT8 inventory_has(UINT8 item_id, UINT8 amount) BANKED;
UINT8 inventory_add(UINT8 item_id, UINT8 amount) BANKED;
UINT8 inventory_remove(UINT8 item_id, UINT8 amount) BANKED;
void inventory_copy_to(UINT8 *dst) BANKED;
void inventory_copy_from(const UINT8 *src) BANKED;
void inventory_menu_show_items_loop(void) BANKED;
UINT16 inventory_get_money(void) BANKED;
void inventory_add_money(UINT16 amount) BANKED;
void inventory_set_money(UINT16 amount) BANKED;
UINT8 inventory_spend_money(UINT16 amount) BANKED;
UINT8 inventory_battle_use_auto(UINT8 active_slot) BANKED;
UINT8 inventory_battle_select_use(UINT8 active_slot) BANKED;
const char *inventory_get_item_name(UINT8 item_id) BANKED;

#endif
