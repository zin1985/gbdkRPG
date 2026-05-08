# rpg128_enemy_bg_tiles

## 目的
32x32敵3体をOBJスプライトではなくBGタイルとして描画し、右端のカットとOAM/スキャンライン制限による欠けを回避する。

## 変更内容
- `sprites.c/.h` に `battle_enemy_bg_load_tiles()` / `battle_enemy_bg_draw_all()` / `battle_enemy_bg_draw_slot()` を追加。
- 戦闘開始時に敵32x32タイル48枚をBG tile 80-127へロード。
- 敵3体をBG座標 x=1,8,15 / y=5 の4x4タイルとして表示。
- 旧OBJ敵スロット0-23は常時hideし、OAMはパーティアイコン、カーソル、将来エフェクト用に残す。
- 被弾時フラッシュは対象BGスロットを一時的に白タイルで消してから再描画。
- 既知の未定義エラー回避として `PARTY_ACTION_ATTACK` / `PARTY_TECH_FLAG_NONE` を `party_runtime.c` に補完。

## Bank 0方針
敵BG描画の実処理は `sprites.c` bank 3 側に置き、`main.c` には最小呼び出しのみ残す。

## 注意
この環境にはGBDK本体がないためビルド未実行。ユーザー環境で `./build.sh` により確認する。
