# HANDOVER rpg078_ui

基準: rpg077_battle

今回の主目的は、戦闘中の再描画による白画面・ちらつき・次ターンずれ・コマンド下段ゴミ表示を修正すること。

次回候補:

- rpg079_target: 攻撃対象選択カーソル
- rpg079_battle_clean: 戦闘メッセージ後の再描画をさらに差分更新化

注意:

- 通常ターン中に `DISPLAY_OFF` を多用しない。
- コマンドカーソル移動でメニュー枠全体を再描画しない。
- Window使用後は `HIDE_WIN`, `move_win(JP_WIN_X, 144u)`, `move_bkg(0,0)` を維持する。
