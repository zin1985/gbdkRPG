# rpg197_inventory_target_proto_fix

## 修正内容
rpg196 のビルドエラー対応。

### 原因
`inventory_battle_select_use()` の中で `inventory_draw_target_popup(item_id, target_slot)` を呼んでいるが、
`inventory_draw_target_popup()` の定義が後ろにあり、プロトタイプ宣言が無かったため、
GBDK/SDCC が暗黙宣言として扱い `too many parameters` になっていた。

### 対応
`inventory.c` 冒頭の static 関数宣言群に以下を追加。

```c
static void inventory_draw_target_popup(UINT8 item_id, UINT8 slot_cursor) BANKED;
```

## 維持している内容
- rpg196 の戦闘中どうぐ対象選択UI
- 装備UI差分更新
- アイテム効果量ランダム化
- 街レイアウト修正
- 町の道具/剣アイコン
- 中央7の村人会話イベント

## 注意
この環境ではGBDK実ビルド未実行。
次のビルドで `inventory_draw_target_popup` の implicit declaration / too many parameters が消えるか確認してください。
