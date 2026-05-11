# rpg204_bank0_transition_split

## 対応内容

rpg202/rpg203系で出た以下の危険警告への対策。

```text
Multiple write ... 0x4000付近
Possible overflow from Bank 0 into Bank 1
```

これは Bank 0 のコードが 0x4000 を越えて Bank 1 側へはみ出した時に出る典型的な警告です。
GBファイルが生成されても白画面化しやすいため失敗扱いにします。

## 修正方針

Bank 0 の `apply_map_event_transition()` が ITILの塔追加で肥大化していたため、
イベント遷移ディスパッチをBanked側へ分離しました。

追加ファイル:

- `map_event_runtime.c`
- `map_event_runtime.h`

配置:

- `map_event_runtime.c` は Bank 17
- `itil_tower_runtime.c` は Bank 16
- ROMバンク数は `-Wl-yo32`

## main.c側の変更

- `apply_map_event_transition()` は薄いラッパーに変更。
- 実際のswitch分岐は `map_event_runtime_apply_transition(event_id, current_area)` へ移動。
- 既存の各遷移処理は `main.c` に残す。
  - 理由: map/camera/player座標など main.c のstatic状態に触るため。
- ただし、Banked側から呼べるように以下の関数は `static` を外した。
  - `enter_town_marker`
  - `enter_port_marker`
  - `enter_dungeon_marker`
  - `enter_ruins_marker`
  - `enter_itil_tower_marker`
  - `advance_itil_tower_marker`
  - `leave_town_marker`
  - `leave_port_marker`
  - `leave_dungeon_marker`
  - `leave_itil_tower_marker`
  - `leave_ruins_marker`

## 維持している内容

- ITILの塔100問化
- `docs/future/ITIL_TOWER_100_QUESTIONS.md`
- Bank 0に問題文を置かない方針
- `-Wl-yo32`
- 既存の街/店/看板/村人スプライト修正

## ビルド確認

次の警告が消えるか確認してください。

```text
Multiple write
Possible overflow from Bank 0 into Bank 1
Write from one bank spans into the next
Overflow by
```

特に今回は Bank 0 overflow 対策なので、romusageで ROM_0 が0x4000未満に収まることを確認してください。
