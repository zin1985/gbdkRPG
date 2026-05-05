# BATTLE HELPER DEFINITION FIX 072

rpg071 added prototypes and call sites for the battle UI helpers, but the helper
function bodies were not emitted into `main.c`.

rpg072 adds definitions for:

- `battle_clear_bg_full()`
- `draw_bkg_box()`
- `draw_battle_enemy_names()`
- `hide_battle_enemy_sprites()`
- `show_battle_enemy_sprites()`
- `battle_copy_enemy_from_data()`
- `battle_select_first_alive()`
- `battle_alive_count()`

No graphics counts, map actors, object kinds, or VRAM load paths were changed.
