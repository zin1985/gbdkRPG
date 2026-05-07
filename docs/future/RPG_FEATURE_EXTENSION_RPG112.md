# RPG112 SaGa風成長・武器熟練・ひらめき土台

## 目的

rpg111_saga_growth_flags で入れた「戦闘中の行動に応じて成長候補を寄せる」考え方を残しつつ、Bank 0 overflow を避けるために追加状態を最小化した版。

## 方針

- rpg110_bank0_turn_trim を基準にする。
- rpg111で追加した専用 `battle_growth_flags` は持たない。
- 既存の `last_growth_type` を戦闘中だけ行動ログとして再利用する。
- 勝利後の `apply_battle_growth()` でログを退避してから `clear_growth_result()` を呼ぶ。
- 成長結果表示は従来通り `last_growth.stat_type` を見る。
- 武器熟練・技習得フラグの土台は `party_runtime.c` 側へ逃がし、main.cに新しいpublic APIを増やさない。

## 追加された挙動

- 主人公が通常攻撃した場合、勝利後に攻撃力または素早さが伸びやすい。
- 主人公が特技を使った場合、勝利後に技力またはMPが伸びやすい。
- 主人公が回復を使った場合、勝利後に回復力が伸びやすい。
- party_runtime.c 内部に武器種、武器熟練、習得技フラグの土台を追加。
- MP消費または回復処理のタイミングで、該当キャラの熟練度・習得フラグを内部更新する。

## 未実装

- 技習得メッセージ
- 技一覧UI
- 技IDごとの効果テーブル
- 熟練度表示
- セーブデータへの熟練・技フラグ保存
- 通常攻撃によるparty_runtime側の武器熟練加算

## Bank 0注意

この版でも必ず romusage を確認すること。`Possible overflow beyond Bank 0` が出た場合は、main.c側の成長ログ処理をさらに削るか、成長判定をparty_runtime/battle_runtime側へ移す。
