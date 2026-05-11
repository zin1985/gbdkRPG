# rpg209_bank0_transition_wrapper_remove

## 対応内容

rpg208でもBank 0 overflowが約0x16 bytes残っていたため、さらにBank 0を削減。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x4015
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

rpg204で `apply_map_event_transition()` の本体はBank 17へ逃がしていたが、
main.c側に薄いラッパー関数がまだ残っていた。

削除:
- `apply_map_event_transition()`

置換:
- `apply_map_event_transition(event_id)`
  → `map_event_runtime_apply_transition(event_id, current_area)`

## 狙い

rpg208時点の超過は20bytes前後なので、ラッパー関数本体＋call分を削ってBank 0を収める。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- 音楽トラック判定のBank 5移動
- 小ラッパー削除
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

これでも残る場合は、次は `activate_heal_spring()` または `open_save_point_menu()` をBanked側へ逃がす。
