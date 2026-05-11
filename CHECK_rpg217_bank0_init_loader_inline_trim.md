# rpg217_bank0_init_loader_inline_trim

## 対応内容

rpg216でもBank 0 overflowが最後に約0x0E bytes残っていたため、Bank 0内の単発初期化/ロード小関数を削除。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x400e
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

呼び出しが1回だけの小関数を削除し、呼び出し元へ直接展開。

削除:
- `init_player_skills()`
- `load_battle_party_icon_sprite_data()`

置換:
- `init_player_skills()`
  - `player_skills.slots[0..3]` への初期値代入を初期化部へ直接展開
- `load_battle_party_icon_sprite_data()`
  - `set_banked_sprite_data(BATTLE_PARTY_ICON_TILE_BASE, 12u, battle_party_display_tiles, BANK(sprite_data_bank));`
  - を戦闘画面セットアップ部へ直接展開

## 狙い

rpg216時点の超過は十数bytesのため、単発小関数のCALL/RET/本体分を削ってBank 0を収める。

## 維持している内容

- rpg216の戦闘小ラッパー削除
- rpg215の回復泉Bank5分離
- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `itil_quiz_bank.c` Bank 18
- `map_event_runtime.c` Bank 17
- `-Wl-yo32`
- 問題文はBank18へ分離済み

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、`battle_hide_command_cursor_obj()` か `battle_reset_bg_origin()` の扱いを見直す。
