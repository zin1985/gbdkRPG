# CHECK rpg123_menu_ui_fix_full_project

## ベース
- rpg122_corner_edge_user_samples

## 変更ファイル
- main.c
- inventory.c
- party_runtime.c

## 修正内容
- 装備画面の文字化け残り対策
  - 可変長の装備名/武器種/メッセージを描画する前に該当領域をクリア。
- 装備画面の下ウィンドウを上へ1マス拡張
  - メイン枠: 0,0,20,15
  - 下枠: 0,15,20,3
- 強さ画面の表示を右へ1マス調整
- 強さ画面の下ウィンドウを上へ1マス拡張
  - メイン枠: 0,0,20,15
  - 下枠: 0,15,20,3
- メインメニュー/もちもの/そうびのカーソル移動時の全体再描画を抑制
  - カーソル移動時はカーソル周辺のみ更新。
  - 一覧変更、仲間切替、装備決定など状態が変わる時だけ再描画。

## 追加した差分ファイル
- docs/changelog/rpg123_main_menu_flicker_fix.diff
- docs/changelog/rpg123_inventory_ui_fix.diff
- docs/changelog/rpg123_party_runtime_ui_fix.diff

## 未対応
- 検討中のRPG新機能大量追加
- 戦闘への装備反映
- 戦闘後成長本格化
- 技ひらめき本格化
- 実機/VBA確認

## ビルド注意
このChatGPT環境には GBDK の lcc が無いため、GB ROMの実ビルド、romusage、Bank 0 overflow確認は未実施。
ユーザー環境で以下を確認すること。

```sh
cd rpg123_menu_ui_fix_full_project
chmod +x build.sh
./build.sh
```

確認ポイント:
- Possible overflow beyond Bank 0
- Write from one bank spans into the next
- ASlink-Warning
- undefined reference
- main.c bytes
- romusage
- VBAでのメニュー/もちもの/そうび/つよさ表示確認

## Bank 0注意
main.c にメインメニュー差分更新用の小関数を追加しているため、Bank 0使用量は必ず再確認すること。
問題が出た場合は、メインメニュー描画の差分更新処理をbanked側へ逃がす方針を検討する。
