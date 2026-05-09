# rpg142_strings_field_ruins_dungeon_bgm

## 目的
- 新アップロードの `sunset_strings_adventure_field_bgm` をフィールド曲にする。
- 直前までフィールド曲だった `sunset_ruins_field_bgm` をダンジョン/遺跡曲として使う。
- 戦闘曲は `heavy_metal_celtic_battle_bgm` のまま維持する。

## 修正内容
- `audio.h` に `AUDIO_TRACK_DUNGEON` を追加。
- `audio.c` に `sunset_strings_adventure_field_bgm` を追加し、フィールド曲に割当。
- `audio.c` で `sunset_ruins_field_bgm` を `AUDIO_TRACK_DUNGEON` に割当。
- `main.c` のダンジョン/遺跡突入時のBGMを `AUDIO_TRACK_DUNGEON` に変更。
- 戦闘後・メニュー復帰時のBGM復元で、町/ダンジョン/フィールドを分岐するよう変更。
- 新BGMファイルは Bank 13、旧フィールド曲兼ダンジョン曲は Bank 14、戦闘曲は Bank 15 のまま。

## 未実行
- この環境ではGBDK本体がないため、実ビルドは未実行。
