# rpg175_bank7_inventory_split_fix

## 修正目的

rpg174 はリンク上は ROM を生成できましたが、以下の危険警告により失敗扱いでした。

```
Warning: Write from one bank spans into the next. 0x1fff6 -> 0x20015 (bank 7 -> 8)
```

これは Bank 7 が境界をまたいでおり、実機・エミュレータ上で不安定化する危険があるため、Bank 7 の容量を逃がす修正を行いました。

## 変更内容

- `inventory.c` を `#pragma bank 7` から `#pragma bank 8` へ移動
- `bank_ids.h` に `ROM_BANK_INVENTORY_RUNTIME_CONCRETE 8u` を追加
- `build.sh` / `Makefile` の出力名を `rpg175_bank7_inventory_split_fix` に更新

## 変更していないこと

- `main.c` には手を入れていません
- UI・戦闘・セーブ仕様は rpg174 のままです
- Bank 0 を増やす修正はしていません
- VRAMロード順、`set_bkg_data()` / `set_sprite_data()` 周りは変更していません

## 期待効果

`inventory.c` は約10KBあり、rpg174 の Bank 7 overflow は約31byte程度だったため、これを Bank 8 に移すことで Bank 7 の境界跨ぎを解消できる可能性が高いです。

## ビルド確認ポイント

```bash
cd rpg175_bank7_inventory_split_fix
chmod +x build.sh
./build.sh
```

特に以下を確認してください。

- `Warning: Write from one bank spans into the next` が消えていること
- `Possible overflow beyond Bank 0` が出ていないこと
- `undefined reference` が出ていないこと
- `main.c bytes` が増えていないこと
- もちもの・所持金・装備UI・セーブ/ロードが引き続き動くこと
