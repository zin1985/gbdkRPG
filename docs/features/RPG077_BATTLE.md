# RPG077_BATTLE

<details open>
<summary>追加内容</summary>

- 味方パラメータ表示を省スペース化しました。
- 敵同時出現数を最大3体へ拡張しました。
- 戦闘用敵OBJスプライト3種を `enemy_tiles` の先頭12タイルに配置しました。
- 戦闘コマンドカーソルを上下左右移動可能にしました。

</details>

<details>
<summary>安全方針</summary>

- 新規BGタイルは追加していません。
- `MAP_GFX_TILE_COUNT` は変更していません。
- 新規actor/NPC/object kindは追加していません。
- 敵スプライトロードは戦闘中のみ `ENEMY0_TILE_BASE` の先頭12タイルに限定しています。
- フィールド復帰時は既存の `restore_field_vram_state()` → `init_map_sprites()` で戻します。

</details>
