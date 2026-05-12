# rpg222_bank0_field_renderer_split

## 対応内容

今後フィールド・街・ダンジョンが増える前提で、Bank0をさらに削減。

### 1. draw_object_map() の本体をBank19へ移動

追加:
- `field_map_render_runtime.c`
- `field_map_render_runtime.h`

Bank:
- Bank19

main.c側は薄い橋渡しだけ。

```c
static void draw_object_map(void) {
    field_map_render_runtime_draw(current_area);
}
```

### 2. Bank19へ移した内容

- 32x32 BGバッファ作成
- 16x16 object kind から8x8 BGタイル4枚への変換
- 危険エリア/港町/町/塔などの見た目分岐
- 港町・町のoverlay tileロード
- `set_bkg_tiles()` によるBG反映

### 3. 残した方がメリットのあるBank0処理

以下は安全性・操作感・main状態依存が強いため今回は残す。

- main loop
- player/camera/OAM状態
- `draw_all_actors()`
- `update_player_movement()`
- `map_input()`
- `battle_input()`
- `battle_update_dirty()`
- `restore_field_vram_state()`
- `change_area_marker()`

### 4. 今後の拡張方針

- 街・ダンジョン追加: `map_event_runtime.c` の `area_transition_defs[]`
- マップ描画種類追加: `field_map_render_runtime.c`
- 大きいミニゲーム: 専用 `xxx_runtime.c` をBanked側
- 問題文/会話文/データ: Banked data file

## ビルド確認ポイント

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Bank19 overflow
```

Bank19が肥大化する場合は、rendererをさらに分割する。
