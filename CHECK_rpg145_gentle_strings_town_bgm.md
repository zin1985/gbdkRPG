# rpg145_gentle_strings_town_bgm

## 目的

街 / 港町BGMを `peaceful_harp_town_bgm` から `gentle_strings_town_bgm` に差し替える。

## BGM割り当て

- FIELD: `sunset_strings_adventure_field_bgm` bank 13
- TOWN: `gentle_strings_town_bgm` bank 12
- DUNGEON: `deep_eerie_motif_dungeon_bgm` bank 11
- RUINS: `sunset_ruins_field_bgm` bank 14
- BATTLE: `heavy_metal_celtic_battle_bgm` bank 15

## Bank 0方針

BGM本体は `#pragma bank 12` の BANKED 関数として保持し、`audio.c` は切替ディスパッチのみ行う。

## ビルド名

`rpg145_gentle_strings_town_bgm.gb`
