# CHECK rpg141_sunset_field_bgm_banked_fix

## 修正内容

- `sunset_ruins_field_bgm.h` 側で `BANKED` 宣言されている3関数について、`sunset_ruins_field_bgm.c` の関数定義側にも `BANKED` を付与。
  - `sunset_ruins_field_bgm_init()`
  - `sunset_ruins_field_bgm_update()`
  - `sunset_ruins_field_bgm_stop()`
- `build.sh` / `Makefile` / `build.bat` の出力名を `rpg141_sunset_field_bgm_banked_fix` に変更。

## 目的

GBDK/SDCC の `error 98: conflict with previous declaration ... __banked` を解消する。

## 注意

- 戦闘BGM・フィールドBGMの組み込み方針は rpg140 から変更なし。
- Bank 0 増加を避けるため、BGM本体は引き続き banked 関数として扱う。
