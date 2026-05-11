# rpg205_bank0_random_encounter_split

## 対応内容

rpg204でもまだ以下のBank 0 overflow警告が残っていたため、さらにBank 0を削減。

```text
Multiple write ... 0x4000付近
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

`main.c` 内の `field_random_encounter_should_start()` は、
ランダム遭遇判定・イベント判定・grace更新・乱数更新をまとめて持っていてBank 0を圧迫していたため、
重い判定本体を `field_feature_runtime.c` 側へ移動。

### 追加

`field_feature_runtime.c`

```c
UINT8 field_feature_random_encounter_should_start_runtime(...)
```

`field_feature_runtime.h`

```c
UINT8 field_feature_random_encounter_should_start_runtime(...) BANKED;
```

### main.c側

`field_random_encounter_should_start()` は、引数を渡す薄いラッパーだけに変更。

## 効果

Bank 0から、ランダム遭遇判定の本体ロジックをBank 5へ逃がす。
rpg204の時点でBank 0は数十〜百数十bytes程度の超過に見えるため、この削減で収まる可能性が高い。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- `-Wl-yo32`
- Bank 0に問題文を置かない方針

## 次の確認ポイント

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、次に `inspect_map_event()` または `check_step_event()` を同じ方式で薄いラッパー化する。
