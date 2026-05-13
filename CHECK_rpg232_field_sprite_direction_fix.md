# rpg232_field_sprite_direction_fix 確認メモ

## 修正目的

rpg231で、フィールド移動中・停止中のプレイヤースプライトが方向に応じて切り替わらず、同じ向きの絵だけが表示される問題を修正した。

## 原因

`load_actor_sprite_data_safe()` の安全ロード処理で、8フレーム分のVRAMスロットへ分割転送していたが、毎回 `tiles` の先頭ポインタを渡していた。

そのため、以下の8枠すべてに同じ16x16フレームが入っていた。

- 下 frame0
- 下 frame1
- 上 frame0
- 上 frame1
- 左 frame0
- 左 frame1
- 右 frame0
- 右 frame1

`set_player_frame(player_dir, walk_frame)` は方向・歩行フレームに応じて正しいタイル番号を指定していたが、VRAM側の中身が全部同じだったため、見た目が変わらなかった。

## 修正内容

`load_actor_sprite_data_safe()` で、フレームごとに転送元ポインタを進めるよう変更。

```c
frame_tiles = tiles + ((UINT16)frame * (UINT16)ACTOR_TILE_COUNT * 16u);
set_banked_sprite_data(
    tile_base + frame * ACTOR_TILE_COUNT,
    ACTOR_TILE_COUNT,
    frame_tiles,
    BANK(sprite_data_bank)
);
```

## 確認ポイント

- 停止中に上下左右それぞれの向きが表示される
- 移動中に上下左右それぞれの歩行フレームが切り替わる
- メニュー/戦闘/街/洞窟からフィールド復帰後も方向表示が壊れない
- NPC/敵スプライトも欠けない
- `Multiple write` / `Possible overflow from Bank 0 into Bank 1` が出ない

## Bank0影響

関数のロジック修正のみで、大きなデータ追加はなし。
Bank0増加は軽微な想定。
