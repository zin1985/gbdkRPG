# rpg224_world_cave_drop_expansion

## 目的
フィールド拡張、海・島・橋、山による分断、4階建て洞窟ダンジョン、隣町、強敵・ボス・宝物、通常/レアドロップ、戦闘勝利時のお金入手を追加する。

## 実装概要
- `AREA_FIELD_EAST` を追加し、洞窟を抜けた先の隣接フィールドとして使用。
- `AREA_CAVE_1`〜`AREA_CAVE_4` を追加し、4階構成の洞窟を実装。
- `AREA_EAST_TOWN` を追加し、隣町に強い道具屋・武具屋を実装。
- 西フィールドを山・海・橋で複雑化。山で東側を分断し、洞窟入口から東フィールドへ抜ける構成に変更。
- 東フィールドは海・島・橋・山を含む別マップとして追加。
- 洞窟4階の奥にボスイベントを配置。
- ボス撃破時に強い宝物として `ITEM_DRAGON_SWORD` / `ITEM_DRAGON_ARMOR` と300Gを付与。
- 戦闘勝利時に所持金を加算。
- 通常ドロップとして `ITEM_HIGH_POTION`、レアドロップとして `ITEM_POWER_SEED` を低確率で付与。
- 洞窟・東フィールドでは強めの敵編成が出るよう `battle_data_load_random_area()` を追加。

## 変更ファイル
- `field_feature_runtime.h`
- `field_feature_runtime.c`
- `map_event_runtime.c`
- `map_data.h`
- `map_data_bank.c`
- `map_data_runtime.c`
- `field_map_render_runtime.c`
- `battle_data.h`
- `battle_data_bank.c`
- `battle_data_runtime.c`
- `shop_runtime.h`
- `shop_runtime.c`
- `main.c`
- `save_runtime.h`

## Bank0方針
- マップ本体は `map_data_bank.c` Bank4 に追加。
- マップ描画の見た目分岐は `field_map_render_runtime.c` Bank19 側。
- 通常遷移は `map_event_runtime.c` Bank17 のテーブルへ追加。
- 強敵/ボスデータは `battle_data_bank.c` Bank10 側。
- 店の商品差し替えは `shop_runtime.c` Bank5 側。
- Bank0に追加したのは、ボス戦開始と勝利報酬付与の最小処理。

## 要確認
こちらの環境ではGBDK `lcc` がないため実ビルド未実施。
ローカルで `./build.sh` を実行し、以下が出ないことを確認する。
- Possible overflow from Bank 0 into Bank 1
- Multiple write
- Write from one bank spans into the next
- Undefined Global
- Bank19 overflow
- 白画面化

## 動作確認項目
1. 起動して白画面にならない。
2. 西フィールドに海・山・橋が表示される。
3. 山で分断され、洞窟入口から洞窟1階へ入れる。
4. 洞窟1階〜4階を階段で移動できる。
5. 洞窟4階から東フィールドへ抜けられる。
6. 東フィールドから隣町へ入れる。
7. 隣町の道具屋・武具屋で強い商品が売られる。
8. 洞窟で強い敵が出る。
9. 洞窟4階のボスを倒すと強い宝物とお金が入る。
10. 通常戦闘勝利時にお金が増える。
11. 通常/レアドロップが入る。
12. セーブ/ロードで新しいボス撃破フラグが保持される。
