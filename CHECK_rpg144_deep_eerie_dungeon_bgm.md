# rpg144_deep_eerie_dungeon_bgm

## 目的

- ダンジョンBGMを `deep_eerie_motif_dungeon_bgm` に差し替える。
- 旧ダンジョン曲 `sunset_ruins_field_bgm` は削除せず、遺跡/予備曲として残す。
- Bank 0を増やさない方針で、追加BGMはBank 11へ配置する。

## BGM割り当て

| 場面 | 曲 | Bank |
|---|---|---|
| フィールド | sunset_strings_adventure_field_bgm | 13 |
| 街/港町 | peaceful_harp_town_bgm | 12 |
| ダンジョン | deep_eerie_motif_dungeon_bgm | 11 |
| 遺跡/旧ダンジョン曲 | sunset_ruins_field_bgm | 14 |
| 戦闘 | heavy_metal_celtic_battle_bgm | 15 |

## 実装メモ

- `AUDIO_TRACK_DUNGEON` は新ダンジョン曲へ変更。
- `AUDIO_TRACK_RUINS` を追加し、旧曲を保持。
- 遺跡突入時は `AUDIO_TRACK_RUINS` を再生する。
- ダンジョン/遺跡/戦闘/街/フィールド復帰時は現在エリアから曲を復元する。
