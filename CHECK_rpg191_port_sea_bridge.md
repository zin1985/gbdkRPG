# rpg191_port_sea_bridge

## 変更内容
- 港町 (`AREA_PORT`) で海と橋の表現を追加。
- ユーザー提供 8x8 スプライトを使い、
  - 海 = 1枚目
  - 橋 = 2枚目
  のタイルとして描画。
- `port_object16_map` / `port_collision16_map` を更新し、港町右側に海エリアと橋(桟橋)を配置。

## 実装方針
- `MAP_GFX_TILE_COUNT` は増やしていません。
- 既存の BG タイル枠を使い、港町表示時だけ
  - `MAP_TILE_DUNGEON_PIT_*` を海タイル
  - `MAP_TILE_CHEST_*` を橋タイル
  に上書きしています。
- 他エリアでは `set_banked_bkg_data()` が既存タイルを再ロードするため、洞窟穴/宝箱の見た目には影響しません。

## 主な編集ファイル
- `sprites.c`
- `sprites.h`
- `main.c`
- `map_data_bank.c`

## 注意
- `main.c` には港町専用の最小限の分岐だけ追加。
- `MAP_GFX_TILE_COUNT` は据え置き。
- この環境では GBDK 実ビルドは未実行。
