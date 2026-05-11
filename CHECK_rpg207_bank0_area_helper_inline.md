# rpg207_bank0_area_helper_inline

## 対応内容

rpg206でもBank 0 overflowが約0x2A bytes残っていたため、さらにBank 0を削減。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x4029
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

`main.c` 内の小さなエリア判定関数を削除。

削除:
- `current_area_is_dangerous()`
- `current_area_is_town_like()`

変更:
- `draw_object_map()` では `area_dangerous` を1回だけ計算して使い回し。
- `actor_visible_in_current_area()` と `try_interact()` は直接条件式に置換。

## 狙い

rpg206時点の超過は非常に小さいため、関数本体＋呼び出し分を削ってBank 0を収める。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- 音楽トラック判定のBank 5移動
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、次は `activate_heal_spring()` または `check_step_event()` をBanked側へ逃がす。
