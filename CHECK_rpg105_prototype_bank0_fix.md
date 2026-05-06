# CHECK rpg105_prototype_bank0_fix

## 目的
rpg104_enemy_sprite_loop_trim のビルドログに出ていた implicit declaration を修正する。

## rpg104 の問題
```text
main.c:687: warning 112: function 'hide_battle_enemy_sprites' implicit declaration
main.c:687: warning 84: 'auto' variable 'hide_battle_enemy_sprites' may be used before initialization
```

## 修正内容
- `hide_battle_enemy_sprites()` のプロトタイプ宣言を `battle_refresh_enemy_sprites_compact()` より前へ移動
- 後方に残っていた同プロトタイプ宣言を削除

## 期待される効果
- warning 112 / warning 84 が消える
- 暗黙宣言による余分なHOME断片が消えれば、rpg104で残っていた `_HOME 0x4000 -> 0x4001` も解消する可能性がある

## 確認観点
- `hide_battle_enemy_sprites` の implicit declaration warning が消えるか
- romusage の Bank 0 overflow warning が消えるか
- 戦闘開始直後の敵スプライト表示が崩れていないか
- 敵撃破後の非表示が維持されているか
- rpg101以降のフィールドスプライト差し替えが維持されているか
