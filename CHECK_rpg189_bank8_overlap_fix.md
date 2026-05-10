# rpg189_bank8_overlap_fix

## 修正内容

rpg188/rpg183系ビルドで発生していた以下の危険警告に対する修正版。

```text
Warning: Multiple write of ... at 0x20000 -> ...
Warning: Possible overflow from Bank 0 into Bank 8
```

原因候補は、`inventory.c` と `ui_icons.c` がどちらも `#pragma bank 8` になっており、Bank 8の出力領域が衝突していたこと。

## 対応

- `inventory.c` は Bank 8 のまま維持
- `ui_icons.c` を Bank 8 から Bank 1 へ移動
- `BANKREF(ui_icons_bank)` / `BANKREF_EXTERN(ui_icons_bank)` を追加
- `build.sh` の出力名を `rpg189_bank8_overlap_fix.gb` に変更
- `main.c` は未変更
- Bank 0 には処理・テーブルを追加していない

## 期待される確認結果

ビルド後、少なくとも以下が消えること。

```text
Multiple write ... 0x20000
Possible overflow from Bank 0 into Bank 8
```

引き続き確認すべき点：

- Bank 1 / Bank 5 / Bank 7 / Bank 8 / Bank 10 の overflow
- `Write from one bank spans into the next`
- `Multiple write`
- `Possible overflow`

