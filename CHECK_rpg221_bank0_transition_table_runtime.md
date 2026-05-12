# rpg221_bank0_transition_table_runtime

## 対応内容

今後フィールド・街・ダンジョンが増える前提で、Bank0を増やさない構造へ寄せた。

### 1. 通常エリア遷移をBank17のテーブル方式へ移動

`map_event_runtime.c` に `AreaTransitionDef area_transition_defs[]` を追加。

今後、通常の街/ダンジョン/出口追加はこのテーブルへ足す。

### 2. Bank0の個別遷移関数を削除

削除:
- `enter_town_marker()`
- `leave_town_marker()`
- `enter_port_marker()`
- `leave_port_marker()`
- `enter_dungeon_marker()`
- `leave_dungeon_marker()`
- `enter_ruins_marker()`
- `leave_ruins_marker()`

### 3. Bank0には汎用遷移関数のみ残す

`change_area_marker()` を `static` 解除し、Bank17から直接呼べるようにした。

変更:
```c
void change_area_marker(UINT8 before_msg, UINT8 area, UINT8 music, UINT8 tx, UINT8 ty, UINT8 dir, UINT8 after_msg)
```

内部では `(Direction)dir` にキャストして既存の `warp_player_to_tile()` を使う。

### 4. ITILの塔などの特殊ケースは維持

以下は特殊描画・報酬・階層処理があるため、今回はmain側の関数を維持。

- `enter_itil_tower_marker()`
- `advance_itil_tower_marker()`
- `leave_itil_tower_marker()`

## 今後の追加方針

街/ダンジョンを増やす場合:

- main.cに新しい `enter_xxx_marker()` を追加しない
- `map_event_runtime.c` の `area_transition_defs[]` に行を追加する
- マップデータは `map_data_bank.c`
- NPCは `actor_runtime.c`
- 町/ダンジョン固有イベントは別のBanked runtimeへ

## 追加ドキュメント

- `docs/design/13_bank0_future_area_strategy.md`

## ビルド確認ポイント

いつも通り以下を確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```
