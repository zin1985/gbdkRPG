# rpg215_bank0_heal_spring_split

## 対応内容

rpg214でもBank 0 overflowが最後に約6 bytes残っていたため、Bank 0から回復泉処理を分離。

今回のログ:

```text
Multiple write of 6 bytes at 0x4000 -> 0x4005
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

`main.c` 内の `activate_heal_spring()` を削除し、回復処理本体をBank 5の `field_feature_runtime.c` へ移動。

追加:
- `field_feature_activate_heal_spring_runtime()` in `field_feature_runtime.c`
- prototype in `field_feature_runtime.h`

移動した処理:
```c
party_heal_all_active();
message_show(MSG_HEAL_SPRING);
```

`draw_all_actors()` はmain.cのstatic状態に依存するため、main.c側の呼び出し元に残した。

## 狙い

rpg214時点ではBank 0の残り超過が6bytes程度なので、
小さい関数本体をBank5へ逃がしてBank 0を収める。

## 維持している内容

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

これでまだ残る場合は、次は `battle_reset_bg_origin()` か `battle_hide_command_cursor_obj()` を見直す。
