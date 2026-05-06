# CHECK rpg094_bank0_trim

## 目的
- rpg093_build_fix は Cコンパイルは通ったが romusage で Bank 0 / _HOME が 0x4010 まで越境したため、Bank 0を削減する。

## 対応内容
- `battle_compact_message()` と `battle_format_damage_message()` を `main.c` から分離。
- 新規 `battle_text.c` / `battle_text.h` を追加。
- `battle_text.c` を bank 7 に配置し、戦闘メッセージ整形処理を Bank 0 から逃がした。
- build対象に `battle_text.c` を追加。
- 出力名を `rpg094_bank0_trim.gb` に更新。

## 期待結果
- main.c の Bank 0 使用量が減る。
- romusage の `Possible overflow beyond Bank 0` が解消される可能性が高い。
- 戦闘UI修正、メッセージ枠、ダメージ数値表示は維持する。

## VBA確認
- ビルドが通ること。
- romusage で Bank 0 overflow が出ないこと。
- 1ターン目の戦闘UIズレが再発しないこと。
- ゆうしゃアイコンが表示されること。
- メッセージ枠が表示されること。
- ダメージ数値が表示されること。
