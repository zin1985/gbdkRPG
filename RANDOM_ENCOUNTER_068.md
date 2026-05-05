# RANDOM ENCOUNTER 068

## Purpose

rpg068 turns field encounters into step-based random encounters.

The existing enemy actor remains in the data table, but is used as a battle
template instead of a visible field object.

## Rules

- Random encounters happen only in AREA_FIELD.
- Random encounters are checked only after a completed player step.
- Special event tiles do not trigger random encounters.
- Town does not trigger random encounters.
- A short grace period is applied after boot, town transition, and battle return.

## Encounter rate

```c
#define RANDOM_ENCOUNTER_RATE 20u
#define RANDOM_ENCOUNTER_GRACE_STEPS 3u
```

The current check is:

```c
random_u8() < RANDOM_ENCOUNTER_RATE
```

This is intentionally simple and uses the existing small LCG plus DIV_REG mixing.

## Battle start effect

The effect is palette flash only:

- no new BG tiles
- no new OBJ sprites
- no WINDOW layer
- no new VRAM data

## OAM

No OAM slots were added.

The field enemy actor is hidden in field mode and is no longer used as an
on-map collision encounter. This keeps random encounters separate from map actors.
