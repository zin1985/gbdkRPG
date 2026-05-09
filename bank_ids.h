#ifndef BANK_IDS_H
#define BANK_IDS_H

/*
 * rpg136 scheduled bank architecture
 *
 * This file documents the intended ROM-bank ownership used by the long-running
 * design reviews. Keep Bank 0 as a tiny non-banked dispatcher. Put bulky
 * runtime/data in banked modules.
 *
 * Current build is expanded to 16 ROM banks (-Wl-yo16) so future systems can
 * be allocated without squeezing every new feature into bank 7.
 */

#define ROM_BANK_CORE                 0u
#define ROM_BANK_BATTLE_RUNTIME       1u
#define ROM_BANK_MESSAGES             2u
#define ROM_BANK_GRAPHICS             3u
#define ROM_BANK_MAP_DATA             4u
#define ROM_BANK_FIELD_FEATURES       5u
#define ROM_BANK_FONT_DATA            6u
#define ROM_BANK_RUNTIME_MISC         7u

#define ROM_BANK_FUTURE_BATTLE        8u
#define ROM_BANK_FUTURE_ENEMY_AI      9u
#define ROM_BANK_FUTURE_SKILL_MAGIC   10u
#define ROM_BANK_FUTURE_EVENT_QUEST   11u
#define ROM_BANK_FUTURE_MENU_UI       12u
#define ROM_BANK_FUTURE_AUDIO_FIELD   13u
#define ROM_BANK_FUTURE_AUDIO         14u
#define ROM_BANK_FUTURE_SAVE_COMPAT   15u

/* rpg143 concrete audio usage aliases */
#define ROM_BANK_AUDIO_TOWN_CONCRETE       12u
#define ROM_BANK_AUDIO_FIELD_CONCRETE      13u
#define ROM_BANK_AUDIO_DUNGEON_CONCRETE    14u
#define ROM_BANK_AUDIO_BATTLE_CONCRETE     15u

#endif
