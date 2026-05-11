# rpg219_itil_harder_choices_same_capacity

## 対応内容

ITILの塔の3択問題が簡単すぎたため、誤答選択肢を「既存の類似したITIL用語」中心に差し替え。

## 方針

- 100問の問題数は維持。
- 正解位置の分散は維持。
- 容量を増やさないため、文字列総量を増やさない。
- 誤答選択肢は、以下のようなITIL用語・近接概念へ変更。
  - インシデント管理
  - 問題管理
  - 変更実現
  - サービス要求
  - リリース管理
  - 展開管理
  - SLA / OLA / UC
  - SVS / SVC
  - 継続的改善
  - サービスデスク
  - 構成管理
  - IT資産管理
  - ナレッジ管理

## 容量確認

- 旧ITIL問題文字列合計: 6179 bytes
- 新ITIL問題文字列合計: 5423 bytes

新しい方が小さいため、Bank18の容量は増えていません。

## 変更ファイル

- `itil_quiz_bank.c`
- `docs/future/ITIL_TOWER_100_QUESTIONS.md`
- `README.md`
- `CHECK_rpg219_itil_harder_choices_same_capacity.md`

## 維持している内容

- Bank 0削減方針
- ITIL問題文はBank18
- `itil_tower_runtime.c` Bank16
- `map_event_runtime.c` Bank17
- `-Wl-yo32`

## 注意

問題文の容量は増やしていないため、Bank0/Bank18の逼迫悪化は基本的にありません。
ただし、最終判定は必ずビルドログとromusageで確認してください。
