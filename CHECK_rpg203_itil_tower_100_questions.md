# rpg203_itil_tower_100_questions

## 対応内容

- ITILの塔の問題を100問化。
- 既存の12問ローテーションを廃止。
- 1階から100階まで、すべて別問題。
- 追加分は88問以上。合計100問。
- 選択肢は3択。
- 正解位置は 0/1/2 に分散。
- 問題文一覧を `docs/future/ITIL_TOWER_100_QUESTIONS.md` に追加。

## Bank対応

100問分の問題文・選択肢は文字列容量が大きいため、`itil_tower_runtime.c` を Bank 10 から Bank 16 へ移動。

- `#pragma bank 16`
- `ROM_BANK_ITIL_TOWER_CONCRETE 16u`
- `-Wl-yo16` → `-Wl-yo32`

Bank 0 は増やさない方針を維持。

## 確認ポイント

ビルド後に以下を確認。

```text
Multiple write
Possible overflow
Write from one bank spans into the next
Bank 16 overflow
Bank 10 overflow
```

`-Wl-yo32` にしたため、実機カートリッジ/フラッシュカートが MBC5 かつ十分なROM容量に対応していることを確認。
