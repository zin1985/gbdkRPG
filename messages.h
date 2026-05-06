#ifndef MESSAGES_H
#define MESSAGES_H

#include <gb/gb.h>

/*
 * rpg055:
 * Actor/dialogue code should use small message IDs in main.c.
 * Actual UTF-8 message strings live in messages_bank.c.
 */
#define MSG_COMMON_NPC  0u
#define MSG_TEST_SHORT  1u
#define MSG_NONE        0xFFu
#define MSG_GROWTH_HP   2u
#define MSG_GROWTH_MP   3u
#define MSG_GROWTH_ATK  4u
#define MSG_GROWTH_DEF  5u
#define MSG_GROWTH_SKILL 6u
#define MSG_GROWTH_HEAL 7u
#define MSG_GROWTH_AGI  8u
#define MSG_ENEMY_STILL 9u
#define MSG_ENEMY_REVIVED 10u
#define MSG_LEAVE_AS_IS 11u
#define MSG_ENTER_TOWN 12u
#define MSG_ARRIVE_TOWN 13u
#define MSG_BACK_FIELD 14u
#define MSG_LEFT_FIELD 15u
#define MSG_LATER 16u
#define MSG_STAY_TOWN 17u
#define MSG_DARK_CAVE 18u
#define MSG_ENEMY_DEFEATED_REVIVE 19u
#define MSG_BATTLE_APPEAR 20u
#define MSG_BATTLE_ATTACK 21u
#define MSG_BATTLE_LOW_MP 22u
#define MSG_BATTLE_SKILL 23u
#define MSG_BATTLE_HP_FULL 24u
#define MSG_BATTLE_HEAL_DONE 25u
#define MSG_BATTLE_ESCAPE_OK 26u
#define MSG_BATTLE_ESCAPE_NG 27u
#define MSG_BATTLE_ENEMY_ATTACK 28u
#define MSG_BATTLE_WIN 29u
#define MSG_BATTLE_LOSE 30u
#define MSG_PROMPT_REVIVE 31u
#define MSG_PROMPT_ENTER_TOWN 32u
#define MSG_PROMPT_EXIT_TOWN 33u
#define MSG_COUNT       34u

void message_show(UINT8 id) NONBANKED;
const char *message_get_buffered(UINT8 id) NONBANKED;

#endif
