# rpg216_bank0_battle_small_wrappers_trim

## 対応内容

rpg215でもBank 0 overflowが約10 bytes残っていたため、Bank 0内の戦闘系小ラッパーをさらに削除。

今回のログ:

```text
Multiple write of 10 bytes at 0x4000 -> 0x4009
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

小さい関数の本体とCALL/RET相当を削るため、以下を呼び出し元へ直接展開。

削除:
- `battle_start_skill_select()`
- `battle_start_ally_target_select(UINT8 skill_id)`
- `player_defend()`
- `battle_current_consume_mp(UINT16 cost)`

置換:
- `CMD_SKILL` 内で、特技選択開始処理を直接実行
- `CMD_DEFEND` 内で、防御処理を直接実行
- 味方対象特技の選択開始処理を直接実行
- MP消費判定は `party_battle_op(PARTY_OP_TRY_CONSUME_MP, ...)` を直接呼び出し

## 追加修正

`itil_quiz_bank.c` の `ITIL_QUIZ_COUNT == 0u` 分岐は常にfalseで unreachable warning の原因だったため削除。

## 維持している内容

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

今回の削減対象は10bytes超過に対して十分大きめなので、収まる可能性が高いです。
