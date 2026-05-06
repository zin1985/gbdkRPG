# CHECK 091 BATTLE CLEANUP

## 目的
- 戦闘画面のウィンドウ位置ずれを再度抑止する
- 戦闘終了後に右上へ残る戦闘用スプライト残骸を消す

## 今回の修正
- `restore_field_vram_state()` で `hide_all_sprites_safe()` を先に実行し、
  戦闘専用OAM(パーティアイコン/カーソル/敵)がフィールド復帰後に残らないよう修正
- `load_map_mode()` でも `hide_all_sprites_safe()` を実行し、初期復帰時の残骸を防止
- `draw_battle_frame()` で `battle_hide_window_and_reset_scroll()` を再実行し、
  フル再構築時に必ず戦闘BG原点へ戻すよう修正
- `battle_update_dirty()` でも `battle_hide_window_and_reset_scroll()` を再実行し、
  dirty更新時にスクロール残りでUIがずれないよう修正

## 期待する改善点
- 戦闘画面の上部/下部ウィンドウがマップスクロールの影響を受けにくくなる
- 戦闘後に右上へ残るスプライトが消える
- マップ復帰時に戦闘専用OAMが残らない
