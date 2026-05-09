# CHECK rpg146_inventory_banked_fix

## 修正目的

ダンジョン内の宝箱など、調べる系オブジェクトで `inventory_add()` を呼んだ際にフリーズする可能性を修正する。

## 原因候補

`inventory.c` は `#pragma bank 7` 配置だが、`inventory_add()` / `inventory_get_count()` / `inventory_remove()` などの公開関数が `BANKED` 宣言になっていなかった。
そのため、Bank 0 側の `main.c` から通常関数として直接呼ばれ、現在選択中のROMバンク次第で別バンクの同一アドレスへ飛ぶ可能性があった。

## 修正内容

- `inventory.h` の公開関数に `BANKED` を追加。
- `inventory.c` の公開関数定義にも `BANKED` を追加し、宣言と定義を一致させた。
- `inventory_menu_show_items_loop()` は既に `BANKED` だったため方針維持。
- 出力ROM名を `rpg146_inventory_banked_fix.gb` に更新。

## 対象関数

- `inventory_clear()`
- `inventory_seed_defaults()`
- `inventory_get_count()`
- `inventory_has()`
- `inventory_add()`
- `inventory_remove()`
- `inventory_copy_to()`
- `inventory_copy_from()`

## 未変更

- 宝箱イベントの内容自体は変更していない。
- 宝箱を開けた後の見た目変更は未対応。
- 戦闘、BGM、NPC、マップ、クエスト処理は未変更。
