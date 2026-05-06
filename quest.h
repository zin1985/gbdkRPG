#ifndef QUEST_H
#define QUEST_H

#include <gb/gb.h>

#define QUEST_MAX 16u

#define QUEST_HERB      0u
#define QUEST_SLIME     1u
#define QUEST_LOST_KEY  2u

#define QST_NONE   0u
#define QST_START  1u
#define QST_PROG   2u
#define QST_READY  3u
#define QST_DONE   4u

BANKREF_EXTERN(quest_bank)

void quest_init(void) BANKED;
UINT8 quest_get_state(UINT8 quest_id) BANKED;
void quest_set_state(UINT8 quest_id, UINT8 state) BANKED;
void quest_start(UINT8 quest_id) BANKED;
void quest_advance(UINT8 quest_id) BANKED;
void quest_ready(UINT8 quest_id) BANKED;
void quest_complete(UINT8 quest_id) BANKED;
UINT8 quest_is_done(UINT8 quest_id) BANKED;
void quest_copy_to(UINT8 *dst) BANKED;
void quest_copy_from(const UINT8 *src) BANKED;

#endif
