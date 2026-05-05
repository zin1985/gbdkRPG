# HANDOVER rpg076_ui

## 基準
rpg075_fixからの修正。

## 変更点
- `battle_hide_window_and_reset_scroll()` 追加
- `battle_prepare_intro_screen()` 追加
- `battle_reset_screen_for_command()` をWindow退避込みに強化
- `update_battle_status()` は安全優先でLCD OFF再描画へ変更
- `enter_battle_screen()` は、開始メッセージ前にコマンドUIを描かない流れへ変更
- 左上パーティ枠を20タイル幅へ変更

## 次回候補
rpg077では、初回UIが安定していれば、コマンド欄の固定幅クリアと敵名欄の複数敵表示を微修正する。
