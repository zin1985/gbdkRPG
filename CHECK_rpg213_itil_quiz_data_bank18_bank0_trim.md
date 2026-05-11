# rpg213_itil_quiz_data_bank18_bank0_trim

## 結論

問題文データ自体はrpg203以降、すでにBank 0ではなく `itil_tower_runtime.c` のBank 16にありました。
そのため、今回の `Possible overflow from Bank 0 into Bank 1` は、問題文そのものがBank 0にあることが原因ではありません。

ただし、今後の安全性のために、問題文テーブルをランタイム処理から分離しました。

## 対応内容

### ITIL問題文データ分離

追加:
- `itil_quiz_bank.c`
- `itil_quiz_bank.h`

配置:
- `itil_tower_runtime.c` = Bank 16
- `itil_quiz_bank.c` = Bank 18

変更:
- `itil_tower_runtime.c` から100問の `itil_quizzes[]` を削除。
- Bank18の `itil_quiz_draw()` / `itil_quiz_is_correct()` / `itil_quiz_count()` 経由で問題を参照。
- これにより、100問分の文字列は完全にBank18側へ隔離。

### Bank 0追加削減

rpg211時点でBank 0 overflowが約20bytes前後残っていたため、小さいbattle系ラッパーも削除。

削除:
- `battle_reposition_enemy_sprites_only()`
- `show_battle_enemy_sprites()`

置換:
- `battle_reposition_enemy_sprites_only()` → `battle_refresh_enemy_sprites_compact(0u)`
- `show_battle_enemy_sprites()` → `battle_refresh_enemy_sprites_compact(1u)`

## 重要メモ

問題文をBank18へ動かしても、Bank 0のoverflowが直接大きく減るわけではありません。
Bank 0の現在の原因は `main.c` の処理本体・呼び出し・ラッパー類です。
今回は問題文分離と同時に、Bank 0削減も行っています。

## ビルド確認ポイント

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

また、Bank18を使うため `-Wl-yo32` は維持。
