# RPG Feature Design Accumulation rpg090

This version keeps the rpg089 skill-slot internal design, but fixes the battle window position before adding larger UI features.

## Battle window position rule
- Battle UI must be drawn at raw BG tile coordinates.
- Battle mode must force BG scroll to (0,0).
- Right-shift experiments from rpg084/rpg085 are not used.
- Window layer remains hidden during battle BG UI rendering.
- Sprite cursor remains OBJ-based.

## Future UI rule
Before adding skill-list UI or item-list UI, preserve this origin rule:
- BG = persistent battle/menu UI
- Window = dialogue/message only
- OBJ = cursor/effects/actors
