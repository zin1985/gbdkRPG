#pragma bank 7

#include <gb/gb.h>
#include "messages.h"
#include "actor_runtime.h"

BANKREF(actor_runtime_bank)

/*
 * rpg106:
 * Actor initialization moved out of main.c's static initializer.
 * This removes roughly 88 bytes from _INITIALIZER/_INITIALIZED pressure.
 *
 * The struct layout below intentionally mirrors main.c.
 * Keep this file updated if Actor/Fighter layout changes.
 */
typedef enum DirectionRuntime {
    DIR_DOWN_RT = 0
} DirectionRuntime;

typedef enum ActorKindRuntime {
    ACTOR_KIND_NPC_RT = 0,
    ACTOR_KIND_ENEMY_RT = 1
} ActorKindRuntime;

typedef struct FighterRuntime {
    const char *name;
    UINT16 max_hp;
    UINT16 hp;
    UINT16 max_mp;
    UINT16 mp;
    UINT8 attack;
    UINT8 defense;
    UINT8 skill_power;
    UINT8 heal_power;
    UINT8 agility;
} FighterRuntime;

typedef struct ActorRuntime {
    ActorKindRuntime kind;
    UINT8 active;
    UINT8 solid;
    UINT8 talkable;
    UINT8 tx;
    UINT8 ty;
    DirectionRuntime dir;
    UINT8 walk_frame;
    UINT8 sprite_base;
    UINT8 tile_base;
    UINT8 message_id;
    FighterRuntime stats;
} ActorRuntime;

#define NPC0_SPRITE_BASE_RT   4u
#define ENEMY0_SPRITE_BASE_RT 8u
#define TEST_ACTOR_SPRITE_BASE_RT 12u

#define NPC0_TILE_BASE_RT     32u
#define ENEMY0_TILE_BASE_RT   64u

extern ActorRuntime actors[3u];

static void actor_runtime_clear_fighter(FighterRuntime *f) BANKED {
    f->name = "";
    f->max_hp = 0u;
    f->hp = 0u;
    f->max_mp = 0u;
    f->mp = 0u;
    f->attack = 0u;
    f->defense = 0u;
    f->skill_power = 0u;
    f->heal_power = 0u;
    f->agility = 0u;
}

void actor_runtime_init_actors(void) BANKED {
    actor_runtime_clear_fighter(&actors[0].stats);
    actors[0].kind = ACTOR_KIND_NPC_RT;
    actors[0].active = 1u;
    actors[0].solid = 1u;
    actors[0].talkable = 1u;
    actors[0].tx = 3u;
    actors[0].ty = 2u;
    actors[0].dir = DIR_DOWN_RT;
    actors[0].walk_frame = 0u;
    actors[0].sprite_base = NPC0_SPRITE_BASE_RT;
    actors[0].tile_base = NPC0_TILE_BASE_RT;
    actors[0].message_id = MSG_COMMON_NPC;

    actor_runtime_clear_fighter(&actors[1].stats);
    actors[1].kind = ACTOR_KIND_ENEMY_RT;
    actors[1].active = 1u;
    actors[1].solid = 1u;
    actors[1].talkable = 0u;
    actors[1].tx = 7u;
    actors[1].ty = 2u;
    actors[1].dir = DIR_DOWN_RT;
    actors[1].walk_frame = 0u;
    actors[1].sprite_base = ENEMY0_SPRITE_BASE_RT;
    actors[1].tile_base = ENEMY0_TILE_BASE_RT;
    actors[1].message_id = MSG_NONE;
    actors[1].stats.name = "";
    actors[1].stats.max_hp = 20u;
    actors[1].stats.hp = 20u;
    actors[1].stats.max_mp = 0u;
    actors[1].stats.mp = 0u;
    actors[1].stats.attack = 5u;
    actors[1].stats.defense = 2u;
    actors[1].stats.skill_power = 0u;
    actors[1].stats.heal_power = 0u;
    actors[1].stats.agility = 5u;

    actor_runtime_clear_fighter(&actors[2].stats);
    actors[2].kind = ACTOR_KIND_NPC_RT;
    actors[2].active = 1u;
    actors[2].solid = 0u;
    actors[2].talkable = 1u;
    actors[2].tx = 10u;
    actors[2].ty = 8u;
    actors[2].dir = DIR_DOWN_RT;
    actors[2].walk_frame = 0u;
    actors[2].sprite_base = TEST_ACTOR_SPRITE_BASE_RT;
    actors[2].tile_base = NPC0_TILE_BASE_RT;
    actors[2].message_id = MSG_TEST_SHORT;
}
