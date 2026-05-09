# rpg147_peaceful_balanced_town_bgm

## 目的

rpg146_inventory_banked_fix を基準に、街 / 港町BGMを `gentle_strings_town_bgm` から `peaceful_balanced_town_bgm` へ差し替える。

## BGM割り当て

- FIELD: `sunset_strings_adventure_field_bgm` bank 13
- TOWN: `peaceful_balanced_town_bgm` bank 12
- DUNGEON: `deep_eerie_motif_dungeon_bgm` bank 11
- RUINS: `sunset_ruins_field_bgm` bank 14
- BATTLE: `heavy_metal_celtic_battle_bgm` bank 15

## 実装修正

- `audio.c` の街BGM呼び出しを `peaceful_balanced_town_bgm_*()` に差し替え。
- `peaceful_balanced_town_bgm.c` を `#pragma bank 12` に配置。
- `.h` と `.c` の公開関数を `BANKED` に統一。
- 384ステップ曲のため、`balanced_pos` と `balanced_town_play_step()` の引数を `uint16_t` に変更。
- build.sh / Makefile / build.bat の出力名を `rpg147_peaceful_balanced_town_bgm.gb` に変更。

## 注意

旧街BGM `gentle_strings_town_bgm.c/.h` はファイルとして残しているが、ビルド対象からは外している。
