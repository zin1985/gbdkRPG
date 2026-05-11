# rpg211_bank0_enter_battle_inline_trim

## 対応内容

rpg210でもBank 0 overflowが約0x18 bytes残っていたため、さらにBank 0を削減。

今回のログ:

```text
Multiple write ... 0x4000 -> 0x4017
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

呼び出しが1回だけの `enter_battle(enemy_index)` ラッパーを削除し、呼び出し元へ直接展開。

削除:
- `enter_battle(UINT8 enemy_index)`

置換内容:

```c
game_mode = MODE_BATTLE;
player_moving = 0u;
init_battle_from_enemy(enemy_index);
enter_battle_screen();
```

## 狙い

rpg210時点の超過は20〜30bytes程度のため、単発ラッパー関数を削ってBank 0を収める。

## 維持している内容

- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `map_event_runtime.c` Bank 17
- ランダム遭遇判定のBank 5移動
- 音楽トラック判定のBank 5移動
- 小ラッパー削除群
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

これでも残る場合は、次は `activate_heal_spring()` か `show_battle_party_icons()` を削る。
