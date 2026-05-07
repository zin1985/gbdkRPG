# CHECK rpg114_saga_system_pack_bank0_guard

## 変更方針
- rpg113_saga_growth_bank0_safe を基準。
- Bank 0を守るため main.c は未変更。
- party_runtime.c内部だけで、種族、LP、重量、陣形、熟練、技/魔法習得フラグ、派生ステータス補正を追加。

## 修正したファイル
- party_runtime.c
- build.sh
- build.bat
- Makefile
- docs/future/RPG_FEATURE_EXTENSION_RPG114.md
- CHECK_rpg114_saga_system_pack_bank0_guard.md

## 修正していない重要ファイル
- main.c
- party_runtime.h
- battle_data_bank.c
- battle_data_runtime.c
- battle_text.c
- messages_bank.c
- messages_runtime.c
- sprites.c
- audio.c
- map_data_bank.c
- map_data_runtime.c

## Bank 0対策
- 新しいpublic APIを追加していない。
- main.cの呼び出し点を増やしていない。
- 既存 party_get_active_fighter / party_damage_active / party_battle_op の内部だけで効果を出す。

## 注意
- WRAMは少し増える。
- Bank 0は増やさない想定だが、最終判断はromusageで行う。
