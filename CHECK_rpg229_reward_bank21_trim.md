# rpg229_bank0_reward_command_ui_trim

## 目的
rpg228 でコンパイルは通るが Bank0 が `0x3d9b -> 0x4000` 付近まで迫り、
`Multiple write` / `Possible overflow from Bank 0 into Bank 1` が残ったため、
戦闘勝利報酬処理を Bank21 へ逃がして Bank0 に安全域を作る。

## 変更点
- `battle_reward_runtime.c` / `.h` を追加
- Bank21: `ROM_BANK_BATTLE_REWARD_CONCRETE`
- 戦闘後のお金・通常ドロップ・レアドロップ・洞窟ボス報酬表示を Bank21 へ移動
- `main.c` の `return_to_map_after_battle()` は Banked 呼び出しだけに縮小
- `battle_show_message()` / `battle_show_damage_message()` は Banked runtime から呼ぶため非static化

## 確認項目
- `Multiple write` が消えること
- `Possible overflow from Bank 0 into Bank 1` が消えること
- 戦闘勝利後に「おかね」メッセージが出ること
- ドロップ時に「アイテムを てにいれた」＋アイテム名が出ること
- 洞窟4階ボス撃破時に竜のけん/竜よろい報酬が出ること
- フィールド復帰で白画面化しないこと


## 追加のBank0削減
- `battle_command_ui_runtime.c` / `.h` を追加
- Bank22: `ROM_BANK_BATTLE_COMMAND_UI_CONCRETE`
- 戦闘コマンドメニュー描画とカーソル描画をBank22へ移動
- main.c側は薄いbridgeのみに縮小

## 追加確認
- 戦闘コマンド `こうげき/まほう/とくぎ/どうぐ/ぼうぎょ/にげる` が表示されること
- 十字キーで `>` カーソルが正しく動くこと
