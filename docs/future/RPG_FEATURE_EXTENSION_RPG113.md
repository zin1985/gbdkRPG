# RPG113: SaGa風成長・武器熟練土台のBank 0再削減版

## 目的
rpg112_saga_mastery_spark は rpg111 より大きく改善したが、romusageで `_GSINIT (0x4000 -> 0x4003)` と `_GSFINAL (0x4004)` がBank 0を5byte超過した。

rpg113では、SaGa風成長・武器熟練の土台を残しつつ、Bank 0側の重複初期化と行動ログ処理をさらに小型化した。

## 変更方針
- rpg111は不採用。
- rpg112は惜しいが、Bank 0 warningが残るため不採用。
- rpg113では rpg112 の機能思想を維持し、Bank 0削減を優先。

## 変更内容
### main.c
- `GROWTH_LOG_ATTACK/SKILL/HEAL` のbit flag方式を廃止。
- 既存の `last_growth_type` に `GROWTH_ATK / GROWTH_SKILL / GROWTH_HEAL` を直接入れる方式へ変更。
- `apply_battle_growth()` の `log_flags` 変数とbit判定を削除。
- `init_battle_from_enemy()` と `init_random_battle_from_field()` に重複していた `party_prepare_battle_members()` と `last_growth_type = GROWTH_NONE` を削除。
- 共通入口 `enter_battle_screen()` で1回だけパーティ同期と成長ログ初期化を行う。

### party_runtime.c
- rpg112の武器種・武器熟練・習得技フラグの最小土台を維持。

## 期待効果
- rpg112で残った5byte程度のBank 0 overflowを解消する狙い。
- 機能を捨てず、初期化重複と行動ログ表現だけを小さくする。

## 動作確認ポイント
- romusageに `Possible overflow beyond Bank 0` が出ないこと。
- 戦闘開始時にパーティHP/MPが同期されること。
- 主人公の通常攻撃後は攻撃/素早さ寄りに成長すること。
- 主人公の特技後は技力/MP寄りに成長すること。
- 主人公の回復後は回復力寄りに成長すること。
- 仲間手動操作の順番が壊れていないこと。
