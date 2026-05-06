# CHECK rpg103_bank0_thunk_trim

## 目的
rpg102_party_bank0_trim で残った Bank 0 overflow warning を解消する。

## rpg102 の状態
romusage で以下が残っていた。

```text
_HOME (0x4000 -> 0x40d8)
_INITIALIZER (0x40d9 -> 0x4130)
_GSINIT (0x4131 -> 0x413c)
_GSFINAL (0x413d -> 0x413d)
```

## 原因仮説
`game_flags.c`, `quest.c`, `inventory.c` はまだ既存処理から未接続だが、全APIを `BANKED` 公開関数として定義していたため、
GBDK/SDCC が banked call 用 trampoline を `_HOME` に生成している可能性が高い。

## 修正内容
- `game_flags.h/c`
  - 未接続APIの `BANKED` 指定を解除
  - `BANKREF_EXTERN` / `BANKREF` を削除
  - `#pragma bank 7` は維持

- `quest.h/c`
  - 未接続APIの `BANKED` 指定を解除
  - `BANKREF_EXTERN` / `BANKREF` を削除
  - `#pragma bank 7` は維持

- `inventory.h/c`
  - 未接続APIの `BANKED` 指定を解除
  - `BANKREF_EXTERN` / `BANKREF` を削除
  - `#pragma bank 7` は維持

## 維持
- `party_runtime.c` は既存戦闘から呼ばれているため `BANKED` 維持
- rpg101 のフィールドプレイヤースプライト差し替え維持
- rpg100 の仲間データ・敵攻撃対象選択維持

## 確認観点
- romusage の Bank 0 overflow warning が消えるか
- build が通るか
- 戦闘UIが崩れないか
- 仲間HP/MP表示が動くか
- フィールドプレイヤースプライトが差し替わっているか

## 将来の注意
`game_flags` / `quest` / `inventory` を main.c や他bankから実際に呼ぶ段階では、
単純に `BANKED` を戻すのではなく、公開APIを少数の集約関数へまとめてから banked call 化すること。
