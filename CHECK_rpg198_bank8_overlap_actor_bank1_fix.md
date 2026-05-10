# rpg198_bank8_overlap_actor_bank1_fix

## 修正目的
rpg197 のビルドで以下の危険警告が出ていたため、Bank 8 の重複書き込みを回避する。

```text
Warning: Multiple write of 605 bytes at 0x20000 -> 0x2025c writes:(0x20000 -> 0x2025c, 0x20000 -> 0x202c3)
Warning: Possible overflow from Bank 0 into Bank 8
```

## 原因推定
rpg194 で `actor_runtime.c` を Bank 8 に移動したが、リンク時に Bank 8 先頭で複数領域が重なっていた。
Bank 8 は現状のビルド構成と相性が悪い可能性があるため、使用を避ける。

## 対応内容
- `actor_runtime.c`
  - `#pragma bank 8` → `#pragma bank 1`
- `bank_ids.h`
  - `ROM_BANK_ACTOR_RUNTIME_CONCRETE 1u` に更新
- 出力名
  - `rpg198_bank8_overlap_actor_bank1_fix.gb`

## 維持している内容
- rpg196 の戦闘中どうぐ対象選択UI
- 装備UI差分更新
- アイテム効果量ランダム化
- 街レイアウト修正
- 道具/剣アイコン
- 中央7の村人会話イベント
- rpg197 の `inventory_draw_target_popup` プロトタイプ修正

## 確認ポイント
次のビルドで以下が消えるか確認。

- `Multiple write`
- `Possible overflow from Bank 0 into Bank 8`
- `Write from one bank spans into the next`
- romusage の各Bank overflow

特に Bank 1 は比較的空いている想定なので、まずはここへ逃がす。
