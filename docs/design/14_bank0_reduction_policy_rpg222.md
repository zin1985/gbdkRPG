# rpg222 Bank0削減方針と残すべき処理

## 方針

Bank0は、今後のフィールド拡張・街追加・ダンジョン追加に備えて、以下だけを残す。

- メインループ
- game_mode / battle_state の入口制御
- プレイヤー座標・カメラ・OAMの即時状態操作
- VBlank/OAM/表示復帰に近い安全優先処理
- Banked側からmain状態へ触るための最小API
- 戦闘dirty updateの中心ループ

逆に、以下はBanked側へ逃がす。

- マップ描画ロジック
- マップ遷移テーブル
- 問題文/会話文/データ
- 街/ダンジョン固有イベント
- ミニゲーム本体
- 店・宿・宝箱などの汎用イベント

## rpg222で実施したこと

### draw_object_map() の重い本体をBank19へ移動

追加:

- `field_map_render_runtime.c`
- `field_map_render_runtime.h`

Bank:

- `field_map_render_runtime.c` は Bank19

main.c側:

```c
static void draw_object_map(void) {
    field_map_render_runtime_draw(current_area);
}
```

Bank19側:

- 32x32 BGバッファ作成
- object kind → metatile選択
- 港町/町の看板・海・橋・壺などのoverlay tileロード
- `set_bkg_tiles()` 実行

## 今後の拡張ルール

### フィールドや街の見た目を増やす場合

main.cの `draw_object_map()` には足さない。

- `field_map_render_runtime.c` に追加
- さらに肥大化する場合は、`field_map_render_table_bank.c` などへ分割

### 街・ダンジョンを増やす場合

main.cに `enter_xxx_marker()` を追加しない。

- `map_event_runtime.c` の `area_transition_defs[]` に追加
- マップデータは `map_data_bank.c`
- NPC配置は `actor_runtime.c`

### 残した方がメリットのあるBank0処理

以下は今はBank0に残す。

- `draw_all_actors()`
  - OAMとactor状態へ強く依存
- `update_player_movement()`
  - 毎フレーム・操作感に直結
- `map_input()`
  - メイン入力入口
- `battle_input()`
  - 戦闘状態機械の中心
- `battle_update_dirty()`
  - dirty updateの要
- `restore_field_vram_state()`
  - DISPLAY/OAM/BG復帰の安全性重視
- `change_area_marker()`
  - Banked遷移テーブルからmain状態を更新する最小API

## 注意

`draw_object_map()` はかなり大きい処理なので、Bank19へ移した効果は大きい。  
一方で、Banked呼び出しになるため、毎フレーム呼ぶ処理にはしない。  
現在の用途はエリア切替・メニュー復帰・表示復帰時中心なので許容。
