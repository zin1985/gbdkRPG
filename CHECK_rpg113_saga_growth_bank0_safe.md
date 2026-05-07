# CHECK rpg113_saga_growth_bank0_safe

## 修正したファイル
- main.c
- build.sh
- build.bat
- Makefile
- docs/future/RPG_FEATURE_EXTENSION_RPG113.md
- CHECK_rpg113_saga_growth_bank0_safe.md

## 修正していない主なファイル
- party_runtime.c / party_runtime.h
- battle_data_bank.c / battle_data_runtime.c / battle_data.h
- battle_text.c
- messages_bank.c / messages_runtime.c / messages.h
- sprites.c / sprites.h
- audio.c / audio.h
- map_data_bank.c / map_data_runtime.c
- actor_runtime.c
- quest.c
- inventory.c

## 意図
rpg112で残った `_GSINIT/_GSFINAL` の5byte Bank 0 overflowを消すため、main.c側の重複初期化とbit flag式行動ログを削減した。

## 要確認
- build.sh 実行後、romusageでBank 0 warningが消えていること。
