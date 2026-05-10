# rpg167_save_bridge_event_flag_fix

## 目的
`rpg166_bank0_save_bridge_split` のビルドで `save_bridge_runtime.c` が `EVENT_FLAG_COUNT` を参照し、未定義識別子でコンパイル失敗していた問題を修正。

## 修正内容
- `save_bridge_runtime.c` の復元ループを `SAVE_EVENT_FLAG_COUNT` 基準に変更。
- `EVENT_FLAG_COUNT` は `main.c` 内部定義のため、BANKED側の `save_bridge_runtime.c` から直接参照しない。
- 出力名を `rpg167_save_bridge_event_flag_fix.gb` に変更。

## Bank 0方針
- Bank 0は増やしていない。
- セーブスナップショット作成・復元処理は引き続き `save_bridge_runtime.c` のBank 5側に置く。

## ローカル確認
```sh
./build.sh
```
