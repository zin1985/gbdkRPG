# CHECK rpg096_first_command_ui_fix

## 目的
戦闘開始後、出現メッセージから最初のコマンドカーソルへ移る瞬間だけUIがズレる問題を修正する。

## 原因想定
- rpg095で下部メッセージ枠を2行化した結果、メッセージ枠が rows 14-17 になった。
- 旧コマンド枠は rows 11-14 で、row 14 がメッセージ枠と重なっていた。
- 最初のメッセージ消去/カーソル表示のタイミングで、この重なりがUIズレとして見えていた。
- また、初回カーソル前に全体再描画を行うと白飛び/ちらつきが出るため、最小整列にする必要があった。

## 修正内容
- コマンド枠を1行上へ移動。
  - 旧: `draw_bkg_box(9, 11, 11, 4)` rows 11-14
  - 新: `draw_bkg_box(9, 10, 11, 4)` rows 10-13
- カーソルOBJのBG座標も1行上へ調整。
- `battle_prepare_first_command_ui()` を新設。
  - `DISPLAY_OFF/ON` なし
  - `battle_clear_bg_full()` なし
  - `draw_battle_frame()` なし
  - `hide_all_sprites_safe()` なし
  - sprite tile data reload なし
- 初回コマンド前だけ `battle_prepare_first_command_ui()` を1回実行。
- 初回整列用に、tile reloadなしの以下を追加。
  - `battle_reposition_party_icons_only()`
  - `battle_reposition_enemy_sprites_only()`

## VBA確認
- 戦闘開始直後、最初のカーソル表示時にUIがズレない。
- 下部2行メッセージ枠とコマンド枠が重ならない。
- コマンド選択中、下部メッセージ文字は出ない。
- 最初のカーソル前の白飛びが増えていない。
- ゆうしゃ/そうりょ/まほうアイコンが消えない。
- 敵スプライトが消えない。
- 攻撃/特技/敵攻撃時は2行メッセージでダメージ数値が表示される。
- 戦闘後にフィールドへ戻れる。
