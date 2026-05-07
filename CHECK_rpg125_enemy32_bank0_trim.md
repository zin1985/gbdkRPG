# rpg125_enemy32_bank0_trim

## 目的
rpg124_enemy_32x32_project で 32x32 戦闘敵スプライトを導入した結果、Bank 0 が約 0x5c bytes 超過したため、main.c の戦闘敵OAM処理を軽量化する。

## 修正内容
- `show_one_battle_enemy_sprite()` の 8個分の `set_sprite_tile()` / `move_sprite()` 直書きをループ化
- `hide_one_battle_body()` の 8個分の `move_sprite()` 直書きをループ化
- 32x32敵スプライト、8x16 OBJ戦闘モード、`battle_enemy_tiles` 自体は維持

## 修正ファイル
- `main.c`

## 修正していないファイル
- `sprites.c`
- `sprites.h`
- `inventory.c`
- `party_runtime.c`
- その他データファイル

## 確認してほしいこと
1. `./build.sh` で romusage の Bank 0 overflow が消えるか
2. 戦闘画面でスライム・こうもり・けものが32x32で表示されるか
3. 3体同時表示時に敵同士・味方アイコン・カーソルが重ならないか
4. 戦闘終了後、フィールドに戻った時にスプライトモードが8x8へ戻るか

## まだ overflow する場合の次候補
- 戦闘用32x32敵表示の補助処理をさらに圧縮
- battle cursor / party icon 周辺のHOMEコードを整理
- main.c内のメニュー再描画ヘルパーをさらに縮小
- 機能追加ではなくBank 0削減のみを先に実施
