# CHECK rpg104_enemy_sprite_loop_trim

## 目的
rpg103_bank0_thunk_trim で残った Bank 0 overflow warning を解消する。

## rpg103 の状態

```text
_HOME (0x4000 -> 0x40d8)
_INITIALIZER (0x40d9 -> 0x4130)
_GSINIT (0x4131 -> 0x413c)
_GSFINAL (0x413d -> 0x413d)
```

## 修正内容
- `main.c` の戦闘パーティアイコン配置重複を `battle_place_party_icons()` に集約
- `main.c` の敵スプライト表示/再配置重複を `battle_refresh_enemy_sprites_compact()` に集約
- 既存と同じ敵配置座標を維持
  - 1体: 72
  - 2体: 56, 96
  - 3体: 36, 76, 116

## 狙い
`rpg096` 以降で安全対策として重複実装していた初回コマンド時の再配置処理を、同じ挙動のまま短くし、Bank 0 の `_HOME` 越境を削る。

## 確認観点
- romusage の Bank 0 overflow warning が消えるか
- 戦闘開始直後の初回コマンドUIズレが再発しないか
- 敵1体/2体/3体の表示位置が変わっていないか
- 敵撃破後にスプライトが消えるか
- ゆうしゃ/そうりょ/まほうのアイコン位置が変わっていないか
- フィールドプレイヤースプライトが rpg101 と同じく差し替わっているか
