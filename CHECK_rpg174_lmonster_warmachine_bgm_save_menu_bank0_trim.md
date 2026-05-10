# rpg174_lmonster_warmachine_bgm_save_menu_bank0_trim

## 目的

ユーザー指摘の直近課題を、Bank 0を極力増やさない方針で修正した版。

## 修正内容

- 小さいメッセージウィンドウ表示時に、全スプライトを消さず、ウィンドウ下端と重なるフィールドOAMのみ一時退避する方式へ変更。
- `dialogue.c` を bank 5 へ移動し、会話処理をBank 0から逃がした。
- 装備ID、武器熟練、習得フラグ、戦意/集中/冒険勘/疲労を `PartySaveState` に追加し、セーブ/ロード対象にした。
- SaveSnapshot構造変更に合わせて `SAVE_VERSION` を2へ更新。
- メインメニュー下部に所持金を表示。
- メニュー/店/セーブ画面で、カメラ座標リセット後のフィールドが一瞬見えないよう、画面OFF中にBGスクロールリセットとUI描画を行い、描画後にDISPLAY_ONするよう変更。
- Lサイズ敵との戦闘時は、新規BGM `boss_hope_despair_7part_finale_soft6_sad7_bgm` を再生。
- 添付ウォーマシン画像を96x32のLサイズBG敵グラフィックに変換し、Lサイズウォーマシン敵を追加。
- 新規ボスBGMは bank 9 に配置し、既存 battle BGM bank 15 の肥大化を避けた。

## Bank 0対策

- `main.c` への追加はLサイズ判定の1行相当に抑制。
- `dialogue.c` を bank 5 へ移動し、会話処理とスプライト重なり退避処理をBank 0外へ移動。
- 大きなウォーマシンLサイズタイルは既存 `sprites.c` bank 3 に配置。
- 新規BGMデータは bank 9 に配置。

## 未実行

この環境には `/opt/gbdk/bin/lcc` がないため、実ビルドとromusage確認は未実行。

## ビルド時確認ポイント

- `Possible overflow beyond Bank 0` が出ないこと。
- bank 3 sprites.c の容量が溢れないこと。
- bank 5 に dialogue/menu/shop/save が増えたため、bank 5 が溢れないこと。
- bank 9 の新規BGMが溢れないこと。
- `shadow_OAM` 参照がGBDK環境で解決されること。
- 既存セーブデータはSAVE_VERSION変更により読み込まれない想定。
