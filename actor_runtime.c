#pragma bank 1

#include <gb/gb.h>
#include "messages.h"
#include "actor_runtime.h"
#include "field_feature_runtime.h"

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

static void actor_runtime_setup_npc(ActorRuntime *actor, UINT8 sprite_base, UINT8 tx, UINT8 ty, UINT8 solid, UINT8 message_id) BANKED {
    actor_runtime_clear_fighter(&actor->stats);
    actor->kind = ACTOR_KIND_NPC_RT;
    actor->active = 1u;
    actor->solid = solid;
    actor->talkable = 1u;
    actor->tx = tx;
    actor->ty = ty;
    actor->dir = DIR_DOWN_RT;
    actor->walk_frame = 0u;
    actor->sprite_base = sprite_base;
    actor->tile_base = NPC0_TILE_BASE_RT;
    actor->message_id = message_id;
}

static void actor_runtime_hide_npc(ActorRuntime *actor, UINT8 sprite_base) BANKED {
    actor_runtime_clear_fighter(&actor->stats);
    actor->kind = ACTOR_KIND_NPC_RT;
    actor->active = 0u;
    actor->solid = 0u;
    actor->talkable = 0u;
    actor->tx = 0u;
    actor->ty = 0u;
    actor->dir = DIR_DOWN_RT;
    actor->walk_frame = 0u;
    actor->sprite_base = sprite_base;
    actor->tile_base = NPC0_TILE_BASE_RT;
    actor->message_id = MSG_NONE;
}

static void actor_runtime_setup_enemy_template(void) BANKED {
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
}

void actor_runtime_apply_area_npcs(UINT8 area) BANKED {
    /* rpg137: Area-specific NPC placement.  Only actor[0] and actor[2]
     * are used for NPCs so actor[1] can remain the legacy enemy/battle
     * template.  This keeps OAM usage stable: player 4 sprites + 2 NPCs * 4.
     */
    actor_runtime_setup_enemy_template();

    if (area == AREA_TOWN) {
        /* rpg162: three wall-side shopkeepers.  All NPCs are solid. */
        actor_runtime_setup_npc(&actors[0], NPC0_SPRITE_BASE_RT, 4u, 5u, 1u, MSG_COMMON_NPC);
        actor_runtime_setup_npc(&actors[1], ENEMY0_SPRITE_BASE_RT, 8u, 5u, 1u, MSG_COMMON_NPC);
        actor_runtime_setup_npc(&actors[2], TEST_ACTOR_SPRITE_BASE_RT, 14u, 5u, 1u, MSG_COMMON_NPC);
    } else if (area == AREA_PORT) {
        actor_runtime_setup_npc(&actors[0], NPC0_SPRITE_BASE_RT, 3u, 12u, 1u, MSG_COMMON_NPC);
        actor_runtime_setup_npc(&actors[2], TEST_ACTOR_SPRITE_BASE_RT, 12u, 5u, 1u, MSG_TEST_SHORT);
    } else if (area == AREA_DUNGEON) {
        actor_runtime_setup_npc(&actors[0], NPC0_SPRITE_BASE_RT, 4u, 5u, 1u, MSG_DUNGEON_NPC_GUIDE);
        actor_runtime_setup_npc(&actors[2], TEST_ACTOR_SPRITE_BASE_RT, 10u, 11u, 1u, MSG_DUNGEON_NPC_WOUNDED);
    } else if (area == AREA_RUINS) {
        actor_runtime_setup_npc(&actors[0], NPC0_SPRITE_BASE_RT, 2u, 4u, 1u, MSG_RUINS_NPC_SCHOLAR);
        actor_runtime_setup_npc(&actors[2], TEST_ACTOR_SPRITE_BASE_RT, 10u, 12u, 1u, MSG_RUINS_NPC_SPIRIT);
    } else {
        actor_runtime_hide_npc(&actors[0], NPC0_SPRITE_BASE_RT);
        actor_runtime_hide_npc(&actors[2], TEST_ACTOR_SPRITE_BASE_RT);
    }
}

void actor_runtime_init_actors(void) BANKED {
    actor_runtime_apply_area_npcs(AREA_FIELD);
}
