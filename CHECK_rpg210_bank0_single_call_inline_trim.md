# rpg210_bank0_single_call_inline_trim

## 対応内容

rpg209でもBank 0 overflowが約0x1C bytes残っていたため、Bank 0内の単発呼び出しラッパーをさらに削除。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x401b
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

呼び出し回数が1回だけの小さい関数を削除し、呼び出し元へ直接展開。

削除:
- `enter_random_battle()`
- `open_save_point_menu()`

置換:
- `enter_random_battle()`
  - `game_mode = MODE_BATTLE;`
  - `player_moving = 0u;`
  - `init_random_battle_from_field();`
  - `enter_battle_screen();`
- `open_save_point_menu()`
  - `save_bridge_build_snapshot(&save_snapshot_work);`
  - `save_runtime_save_select(&save_snapshot_work);`
  - `restore_field_vram_state();`

## 狙い

rpg209時点の超過は約30bytes未満のため、単発ラッパーの本体・CALL・RET相当を削ってBank 0を収める。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- 音楽トラック判定のBank 5移動
- 小ラッパー削除
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

これでも残る場合は、次は `activate_heal_spring()` を一部直接展開するか、さらに小さいbattle系ラッパーを削る。
