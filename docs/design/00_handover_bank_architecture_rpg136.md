# rpg136 Scheduled Bank Architecture Pack

## 目的

スケジュール検討で積み上げたバンク構想を、現行プロジェクトへ反映するための整理版です。
今回の主目的は、機能を大量追加する前に「Bank 0 を増やさない土台」を固定することです。

## 現在反映した内容

- 出力ROM名を `rpg136_scheduled_bank_architecture_pack.gb` に変更。
- ROMバンク数を `-Wl-yo16` に拡張。
- `bank_ids.h` を追加し、今後のバンク所有方針をコード側にも明文化。
- `field_feature_runtime.c` を bank 7 から bank 5 へ移動。
- `docs/design/` と `docs/future/` に、将来の設計メモ格納先を作成。
- `tools/bank_architecture_check.py` を追加し、rpg136のバンク構想が崩れていないか確認できるようにした。

## バンク割り当て方針

| Bank | 用途 |
|---:|---|
| 0 | main loop / input / scene dispatcher / tiny common helpers |
| 1 | 将来の battle runtime |
| 2 | messages bank |
| 3 | sprites / graphics data |
| 4 | map data |
| 5 | field/world feature runtime |
| 6 | font data |
| 7 | 現行misc runtime, quest, party, inventory, actor, battle data |
| 8 | future battle formula / growth / spark |
| 9 | future enemy AI / rank / encounter |
| 10 | future skill / magic / status |
| 11 | future event / quest VM |
| 12 | future menu / UI |
| 13 | future minigame / challenge |
| 14 | future audio / BGM / SE |
| 15 | future save / compatibility / archive |

## Bank 0保護ルール

Bank 0に追加してよいものは、以下だけです。

- メインループ
- 入力
- 最小限のscene dispatcher
- VBlank / LCD周辺の最小処理
- 小さい共通状態
- banked関数呼び出し入口

Bank 0に追加してはいけないものは以下です。

- 敵データ
- 技データ
- 魔法データ
- 会話本文
- クエスト本文
- NPC定義
- マップ配列
- ボスAI
- メニュー本体
- ミニゲーム本体
- 大型switch文

## 今回あえて実装しなかったこと

スケジュール検討で出した未来機能を、すべて実ゲーム処理として一気に実装することはしていません。
理由は、現時点でBank 0とWRAMがかなり厳しく、未検証の大機能を同時投入すると白画面化・Bank overflow・OAM/VRAM破綻の危険が高いためです。
今回のrpg136では、まず安全なバンク構想と将来の受け皿を実装しています。
