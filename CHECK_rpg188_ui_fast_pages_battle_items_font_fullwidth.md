# CHECK rpg188_ui_fast_pages_battle_items_font_fullwidth

## 対応内容

- 全角英数字を美咲フォントテーブルへ復旧
- 十字キー押しっぱなし時のカーソルリピートを追加
  - メインメニュー
  - 道具欄
  - 装備候補
  - パラメーター画面
- 道具欄は既存の2列・ページ式キャッシュを維持
- 装備候補もページ式の候補表示へ寄せ、カーソル移動時の全画面再描画を抑制
- 戦闘コマンドに「にげる」を復活
- 戦闘中「どうぐ」を選択式に変更
- パラメーター画面にキャラスプライト表示を追加
- パラメーター画面を3ページ化
  - 1ページ目: 基本パラメーター
  - 2ページ目: 熟練度。武器種アイコン+数値中心
  - 3ページ目: 特技/魔法一覧
- 上/下ページがある場合、▲/▼を表示
  - 道具
  - 装備候補
  - パラメーター
- battle_data_bank.c を Bank 7 から Bank 10 に移動し、Bank 7の余裕を確保
- main.c の大型コメントを削除してソースサイズを削減

## Bank 0方針

- main.cへの追加は戦闘コマンド復帰・戦闘道具呼び出しの最小限に留めた
- 戦闘中道具選択の本体処理は inventory.c Bank 8 側
- パラメーター/装備UI処理は party_runtime.c Bank 7 側
- フォント本体は Bank 6 のまま

## 静的確認

- gccスタブによる syntax check 対象:
  - inventory.c
  - party_runtime.c
  - menu_runtime.c
  - main.c
- prebank_check.py:
  - main.c bytes: 78520
  - main.c の direct set_bkg_data / set_sprite_data なし

## 注意

この環境には /opt/gbdk/bin/lcc がないため、実GBDKビルドは未実行。
ビルド時は Bank 7 / Bank 8 / Bank 10 / Bank 6 の overflow を確認すること。
