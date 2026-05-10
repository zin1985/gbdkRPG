# rpg193_bank8_clean_inventory_bank11

## 目的
rpg189以降で出ていた Bank 8 の `Multiple write` / `Possible overflow from Bank 0 into Bank 8` 警告対策。

## 対応内容
- `inventory.c` を Bank 8 から Bank 11 へ移動。
- `bank_ids.h` の `ROM_BANK_INVENTORY_RUNTIME_CONCRETE` を `11u` に更新。
- `build.sh` の出力名を `rpg193_bank8_clean_inventory_bank11.gb` に更新。
- `build.sh` にビルド前の中間生成物削除を追加。
  - `*.rel`, `*.asm`, `*.lst`, `*.map`, `*.gb`, `*.sym`, `*.noi` などを削除。
  - 旧バンク設定の `.rel` が残ってリンクされる可能性を潰すため。

## 継承内容
- rpg190: カーソル統一
- rpg191: 港町の海/橋
- rpg192: ツボスプライト差し替え

## Bank 0方針
- `main.c` の大型処理追加はしていません。
- `MAP_GFX_TILE_COUNT` は増やしていません。
- Bank 8を実質空け、今後の重複警告を避ける方針です。

## ビルド確認ポイント
以下が消えること:
- `Multiple write`
- `Possible overflow from Bank 0 into Bank 8`
- `Write from one bank spans into the next`
