# AREA ACTOR POLICY 067

## Purpose

rpg066 fixed the message freeze and is now treated as the stable base.

rpg067 adds only a small actor visibility rule so town and field can be separated
without adding new actors, new sprites, new object kinds, or new BG tiles.

## Rules

### AREA_FIELD

- ENEMY actors are visible.
- NPC actors are hidden.
- Hidden NPC actors do not block movement.
- Hidden NPC actors cannot be talked to.

### AREA_TOWN

- NPC actors are visible.
- ENEMY actors are hidden.
- Hidden enemy actors do not block movement.
- Hidden enemy actors cannot trigger battle.

## OAM behavior

No OAM slots are added.

Existing slots remain:

```text
0-3    Player
4-7    Existing NPC slot
8-11   Existing enemy slot
12-15  Existing extra/test actor slot
16-39  Reserved
```

Hidden actors are moved to (0,0) using the existing hide_actor_sprite() path.

## What this does not do

- It does not add a town NPC.
- It does not add a new field enemy.
- It does not add new message strings.
- It does not add new BG tiles.
- It does not change MAP_GFX_TILE_COUNT.
- It does not change set_bkg_data()/set_sprite_data() paths.

## Why this is safe

The change is limited to already-existing actor visibility checks:

- draw_all_actors()
- actor_at_tile()
- find_talkable_actor()

This makes area separation explicit while keeping the graphics pipeline unchanged.
