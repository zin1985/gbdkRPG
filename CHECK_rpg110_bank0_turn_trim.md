# CHECK rpg110_bank0_turn_trim

## 目的
rpg109_manual_party_control の手動仲間操作を維持しつつ、Bank 0 overflow warning を解消する。

## rpg109 の問題
```text
_HOME        (0x4000 -> 0x4012)
_INITIALIZER (0x4013 -> 0x4013)
_GSINIT      (0x4014 -> 0x401f)
_GSFINAL     (0x4020 -> 0x4020)
```

## 修正内容
- `battle_current_consume_mp()` を小型化
  - `party_battle_op(PARTY_OP_TRY_CONSUME_MP, ...)` の結果をそのまま返す
  - `player_battle.mp` への旧ミラー更新を削除
- `enemy_turn()` を小型化
  - `target_id` 変数を削除
  - `player_battle.hp` への旧ミラー更新を削除

## 根拠
rpg109では以下が戦闘中の実体になっている。
- HP/MP表示: `party_get_active_hp/mp()`
- 被ダメージ: `party_damage_active()`
- MP消費/回復: `party_battle_op()`
- 行動者取得: `battle_load_current_actor()` が `party_get_active_fighter()` から読む

そのため、旧単独ゆうしゃ戦闘用の `player_battle.hp/mp` 二重同期は不要。

## 確認観点
- Bank 0 overflow warning が消えるか
- ゆうしゃ → そうりょ → まほう の手動選択が維持されているか
- そうりょ/まほうのMP消費がUIに反映されるか
- 敵攻撃でゆうしゃHPが減るか
- HP0の仲間がスキップされるか
- 戦闘UI位置が崩れないか
