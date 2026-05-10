# rpg183_bank7_game_flags_split_fix

## 目的

rpg182 のビルドで発生した Bank 7 -> 8 のはみ出しを解消するため、Bank 7 に残っていた小型 runtime の `game_flags.c` を Bank 10 へ移動した。

## 変更内容

- `game_flags.c` の `#pragma bank 7` を `#pragma bank 10` に変更
- `bank_ids.h` に `ROM_BANK_GAME_FLAGS_CONCRETE 10u` を追加
- `main.c` は未変更
- rpg182 の2列アイテム表示、スクロール、追加アイテム、アイコン表示は維持

## 狙い

- Bank 7 の 31 byte 程度の overflow を解消
- Bank 0 を増やさずに容量を逃がす

## ビルド確認ポイント

- `Write from one bank spans into the next` が消えること
- `Multiple write` が消えること
- `Possible overflow from Bank 0 into Bank 8` が消えること
- Bank 10 の overflow が出ないこと
