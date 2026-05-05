# BANK DESIGN PLAN 051

## Current diagnosis

rpg046/rpg047/rpg048b/rpg049 worked, but rpg050 white-screened after adding only a very short message string.

Likely cause:

- main.c and literal/string data are near a fragile ROM placement limit.
- Adding a small string changes linker layout enough to boot-fail.
- The project needs bank-aware data separation before adding more content.

## Immediate rule

Until bank usage is measured:

- Do not add new strings to main.c.
- Do not add new branch logic to try_interact().
- Do not add new BG tiles.
- Do not increase MAP_GFX_TILE_COUNT.
- Do not add new sprite sheets.

## Target file split

Recommended future layout:

```text
main.c
  main loop, state transitions, input only

messages.h
  message IDs and message_show() declaration

messages_bank.c
  banked message strings

messages_runtime.c
  NONBANKED wrapper that copies banked string to WRAM then calls dialogue_message()

actors_data.c
  actor initial data, after messages are stable

map_field.c
  field map data, after actor data is stable

graphics_bg.c
  BG tile data, after map data is stable

graphics_obj.c
  OBJ sprite tile data, later

battle.c
  battle processing, later
```

## First safe banking step

rpg052 candidate:

- Add `messages.h`
- Add empty/minimal `messages_bank.c`
- Add empty/minimal `messages_runtime.c`
- Include them in build scripts
- Do not call them from main.c yet

The goal is to prove that adding extra compilation units is safe before moving strings.

## Second safe banking step

rpg053 candidate:

- Move only the already-existing common NPC message into messages_bank.c.
- Keep message text identical.
- Add a NONBANKED message_show() wrapper.
- Change only one existing call site.

Do not add new message content until after this passes.
