# CHECK rpg115_original_growth_no_lp_bank0_guard

## 基準

- rpg113_saga_growth_bank0_safe が動作確認済み
- rpg114のうちLP以外の土台を引き継ぎ
- LPは削除

## 変更した箇所

- party_runtime.c
- build.sh / build.bat / Makefile の出力名
- docs/future/RPG_FEATURE_EXTENSION_RPG115.md

## 変更していない箇所

- main.c
- party_runtime.h
- battle_data_bank.c / battle_data_runtime.c
- battle_text.c
- messages_bank.c / messages_runtime.c
- sprites.c
- audio.c
- map_data_bank.c / map_data_runtime.c
- actor_runtime.c
- quest.c
- inventory.c

## 確認ポイント

- Bank 0 overflow がないこと
- `Possible overflow beyond Bank 0` が出ないこと
- `Write from one bank spans into the next` が出ないこと
- 戦闘中の仲間操作がrpg113と同じく動くこと
- HP0時にLP処理が走らないこと
- 回復、特技、被弾後も白画面化しないこと
- WRAM_LOが極端に悪化していないこと
