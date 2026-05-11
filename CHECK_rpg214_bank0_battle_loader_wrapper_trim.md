# rpg214_bank0_battle_loader_wrapper_trim

## 対応内容

rpg213でもBank 0 overflowが約0x10 bytes残っていたため、Bank 0内の戦闘ロード系ラッパーを削除。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x400f
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

Bank 0に残っていた戦闘初期化系の小ラッパーを削除し、呼び出し元へ直接展開。

削除:
- `show_battle_party_icons()`
- `load_battle_enemy_sprite_data()`

置換:
- `show_battle_party_icons()`
  - `battle_place_party_icons()` へ置換
  - パーティアイコンタイルは直前に `load_battle_party_icon_sprite_data()` で既にロード済みのため、二重ロードを避ける
- `load_battle_enemy_sprite_data()`
  - `battle_enemy_bg_load_tiles_for_formation(battle_enemy_count, battle_enemy_sprite_kinds, battle_enemy_size_kinds)` へ直接置換

## 問題文について

ITIL問題文はrpg213で `itil_quiz_bank.c` Bank18へ分離済み。
今回のBank 0 overflowは問題文ではなく、main.c側の残り数十bytesが原因。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `itil_quiz_bank.c` Bank 18
- `map_event_runtime.c` Bank 17
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

これでも残る場合は、次は `battle_reset_bg_origin()` か `battle_hide_command_cursor_obj()` の扱いを見直す。
