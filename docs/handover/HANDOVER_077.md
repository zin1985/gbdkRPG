# HANDOVER_077

## 最新ZIP

- `rpg077_battle.zip`

## 変更概要

- 味方パラメータ表示を `/最大値` なしの省スペース表示へ変更。
- 敵同時出現数を最大3体へ拡張。
- 戦闘用敵スプライト3種を追加。
- コマンドカーソルを上下左右移動へ変更。

## 次に見る場所

- `main.c`
  - `draw_party_status_box()`
  - `load_battle_enemy_sprite_data()`
  - `show_battle_enemy_sprites()`
  - `battle_input()`
- `battle_data.h`
  - `BATTLE_DATA_MAX_ENEMIES`
- `battle_data_bank.c`
  - `BattleEncounterTemplate`
  - `encounter_table`
- `sprites.c`
  - `enemy_tiles`

## 注意

3体表示ではOAM 0-11を使います。フィールド復帰時は必ず `restore_field_vram_state()` が通ることを確認してください。
