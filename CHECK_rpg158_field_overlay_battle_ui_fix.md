rpg158_field_overlay_battle_ui_fix

- Field idle overlay now shows HP/MP for all three active members.
- Field idle overlay includes a 16x16 minimap generated as one map chip = one pixel.
  The map uses four Game Boy shades for object kind differences and marks the player position.
- Battle bottom UI was moved down to the last rows and now shares the area with messages.
  Command/enemy-name panels are restored when no message is active.
- Skill/ally selection windows were moved to the same bottom area so target/message transitions cover them cleanly.
- The full-width top party status box was split into three compact boxes to avoid the white-strip look on the second BG row.
