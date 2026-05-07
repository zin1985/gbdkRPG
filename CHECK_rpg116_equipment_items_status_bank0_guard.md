# CHECK rpg116_equipment_items_status_bank0_guard

## 変更概要

rpg115 を基準に、LPなし方針を維持したまま、アイテム・武器・防具・アクセサリ・装備スロット・装備由来の派生ステータス・仲間別ステータス表示・もちもの表示・装備表示を追加した。

## 変更したファイル

- main.c
- inventory.h
- inventory.c
- party_runtime.h
- party_runtime.c
- build.sh
- build.bat
- Makefile
- docs/future/RPG_FEATURE_EXTENSION_RPG116.md
- precheck_output_rpg116.log
- rpg116_equipment_items_status.diff

## 変更していない主要ファイル

- sprites.c / sprites.h
- battle_data_bank.c / battle_data_runtime.c
- battle_text.c
- messages_bank.c / messages_runtime.c
- map_data_bank.c / map_data_runtime.c
- actor_runtime.c
- quest.c
- audio.c

## Bank 0注意

- main.c からステータス詳細表示を削り、bank 7側へ寄せた。
- main.c bytes は prebank_check で 95608。
- GBDK本ビルド・romusage はこの環境では未実行。

## ローカル確認項目

- ビルド成功すること
- Possible overflow beyond Bank 0 が出ないこと
- メニュー > つよさ で LEFT/RIGHT により仲間切替できること
- メニュー > もちもの でやくそう等が表示されること
- メニュー > そうび で仲間別の武器・防具・装飾が表示されること
- 戦闘中、派生ステータスが壊れていないこと
- 戦闘後にフィールド復帰できること
