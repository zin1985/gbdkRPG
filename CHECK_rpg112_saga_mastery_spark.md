# CHECK rpg112_saga_mastery_spark

## 基準

- rpg110_bank0_turn_trim を基準に再生成。
- rpg111_saga_growth_flags はBank 0 overflowのため採用不可。

## 修正したファイル

- main.c
- party_runtime.c
- build.sh
- build.bat
- Makefile
- docs/future/RPG_FEATURE_EXTENSION_RPG112.md

## 修正していない主なファイル

- audio.c / audio.h
- sprites.c / sprites.h
- dialogue.c / dialogue.h
- jpfont.c / jpfont.h
- misakiUTF16.c / misakiUTF16.h
- messages_bank.c / messages_runtime.c / messages.h
- banked_graphics.c / banked_graphics.h
- map_data_bank.c / map_data_runtime.c / map_data.h
- font_data_bank.c / font_data_runtime.c
- battle_data_bank.c / battle_data_runtime.c / battle_data.h
- battle_text.c
- game_flags.c
- quest.c
- inventory.c
- actor_runtime.c

## ローカル確認

```sh
cd rpg112_saga_mastery_spark
chmod +x build.sh
./build.sh
```

必ず以下が出ないことを確認する。

```text
Possible overflow beyond Bank 0
Write from one bank spans into the next
ASlink-Warning
```

## 判定

ChatGPT環境には /opt/gbdk/bin/lcc がないため、GBDK本ビルドとromusage確認は未実行。
静的チェック結果は `precheck_output_rpg112.log` を参照。
