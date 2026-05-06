# CHECK rpg097_foundation_features

## 目的
スレッドで検討していた将来RPG機能のうち、低リスクで追加できる基礎モジュールを実装する。

## 追加実装
- `game_flags.c/h`
  - 512bit / 64byte のイベントフラグ管理
  - set / clear / get
  - save用 copy_to / copy_from

- `quest.c/h`
  - 16件分のクエスト状態管理
  - NONE / START / PROG / READY / DONE
  - HERB / SLIME / LOST_KEY の初期ID

- `inventory.c/h`
  - 64種類分の簡易所持数管理
  - 最大99個スタック
  - HERB / POTION / CASTLE_KEY の初期ID

## 今回あえて接続していないもの
- NPC会話への接続
- 宝箱イベントへの接続
- セーブデータ本体への組み込み
- 戦闘ドロップへの接続
- メニューUIへの接続

## 理由
現在の戦闘UIがかなり安定しているため、既存挙動を壊さないよう、まずはbanked moduleとして安全に追加する。

## 確認観点
- ビルドが通るか
- Bank 0 warning が悪化していないか
- フィールド表示が壊れないか
- 戦闘UIが rpg096 と同じく安定しているか
- 既存戦闘後のフィールド復帰が壊れていないか
