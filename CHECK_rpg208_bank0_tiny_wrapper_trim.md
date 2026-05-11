# rpg208_bank0_tiny_wrapper_trim

## 対応内容

rpg207でもBank 0 overflowが約0x22 bytes残っていたため、Bank 0内の小さいラッパー関数を削除。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x4021
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

Bank 0内の「1行だけ呼び出す」小さい関数を削除し、呼び出し元を直接呼び出しに置換。

削除:
- `battle_reposition_party_icons_only()`
- `player_skill()`
- `player_use_skill()`
- `reload_field_bg_tiles()`

置換:
- `battle_reposition_party_icons_only()` → `battle_place_party_icons()`
- `player_skill()` → `player_use_skill_on_target(battle_selected_skill, battle_party_turn_slot)`
- `reload_field_bg_tiles()` → `draw_object_map(); apply_camera_scroll();`

## 狙い

rpg207時点の超過は約30〜40bytes程度なので、小さい関数本体とcall分を消してBank 0を収める。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- 音楽トラック判定のBank 5移動
- エリア判定関数削除
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、次に `activate_heal_spring()` や `open_save_point_menu()` を削る/移動する。
