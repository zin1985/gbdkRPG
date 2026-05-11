# rpg206_bank0_music_helper_split

## 対応内容

rpg205でもBank 0がまだ約0x30 bytesほどはみ出していたため、さらにBank 0を削減。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x4030
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

`main.c` の `current_area_music_track()` をBank 0から削除し、
`field_feature_runtime.c` 側の `field_feature_music_track(area)` へ移動。

### 追加

`field_feature_runtime.h`

```c
UINT8 field_feature_music_track(UINT8 area) BANKED;
```

`field_feature_runtime.c`

```c
UINT8 field_feature_music_track(UINT8 area) BANKED;
```

### main.c側

`audio_play_music(current_area_music_track())`

を

```c
audio_play_music(field_feature_music_track(current_area))
```

へ変更。

## 狙い

rpg205時点のoverflowはかなり小さくなっているため、
音楽トラック判定の小さい関数をBank 5へ逃がしてBank 0をさらに数十〜百数十bytes削る。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、次に `activate_heal_spring()` や `check_step_event()` の一部をBanked側へ移動する。
