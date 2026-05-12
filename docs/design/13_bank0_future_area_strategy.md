# Bank0を増やさないためのマップ/街/ダンジョン拡張方針

## 結論

今後フィールド・街・ダンジョンを増やす場合、Bank0の `main.c` に専用関数を増やさない。

rpg221から、通常のエリア遷移は `map_event_runtime.c` のBank17にある `area_transition_defs[]` テーブルで管理する。

## 変更点

### これまで

街やダンジョンを増やすたびに、Bank0側へ以下のような関数が増えていた。

```c
enter_town_marker()
leave_town_marker()
enter_dungeon_marker()
leave_dungeon_marker()
```

これはBank0を少しずつ圧迫する。

### rpg221以降

通常の遷移はBank17側のテーブルに追加する。

```c
static const AreaTransitionDef area_transition_defs[] = {
    { MAP_EVENT_TOWN, TRANS_AREA_ANY, MSG_ENTER_TOWN, AREA_TOWN, AUDIO_TRACK_TOWN, 2u, 13u, DIR_DOWN_U, MSG_ARRIVE_TOWN, QUEST_OP_NONE, 0u },
};
```

Bank0側に残すのは、以下のみ。

- 汎用 `change_area_marker()`
- プレイヤー座標・カメラ・描画復帰などmain.cの状態に強く依存する処理
- ITILの塔のような特殊ケースの最小限の入口

## 今後の追加ルール

### 街を増やす場合

1. `AREA_*` を追加
2. `MAP_EVENT_*` を追加
3. `field_feature_map_event()` に入口/出口イベント座標を追加
4. `map_event_runtime.c` の `area_transition_defs[]` に遷移定義を追加
5. マップデータは `map_data_bank.c` 側へ置く
6. NPC配置は `actor_runtime.c` 側へ置く

Bank0へ `enter_new_town_marker()` のような関数を追加しない。

### ダンジョンを増やす場合

街と同じ。通常の入口/出口は `area_transition_defs[]` で管理する。

敵テーブル・宝箱・イベント・NPCはそれぞれBanked側へ置く。

### 特殊イベントを増やす場合

ITILの塔のように、状態・報酬・問題・ミニゲームを持つ場合は専用runtimeをBanked側に作る。

例:

```text
tower_runtime.c       Bank16など
tower_data_bank.c     Bank18以降
tower_event_runtime.c Bank17以降
```

Bank0には、呼び出しだけ残す。

## rpg221で削除したBank0関数

削除してBank17へ寄せたもの:

- `enter_town_marker()`
- `leave_town_marker()`
- `enter_port_marker()`
- `leave_port_marker()`
- `enter_dungeon_marker()`
- `leave_dungeon_marker()`
- `enter_ruins_marker()`
- `leave_ruins_marker()`

## 期待効果

今後、街・港町・ダンジョン・遺跡を増やしても、通常遷移だけならBank0の増加をほぼ避けられる。

Bank0は、広いフィールド化・街追加・ダンジョン追加ではなく、ゲーム全体の最小制御だけを担当する。
