# rpg127_feature_pack_safe

## 追加した機能
1. 通常攻撃でも装備武器の熟練度が伸びるようにした
   - `party_battle_op(PARTY_OP_NOTE_ATTACK, ...)` を追加
   - 通常攻撃時に `party_gain_mastery()` と `party_note_action(PARTY_ACTION_ATTACK)` を呼ぶ

2. 戦闘後にアクティブメンバー全員へ軽量な成長判定を追加
   - `party_after_battle_growth(enemy_rank, random_seed)` を追加
   - 最後の行動傾向に応じて、HP/MP/攻撃/防御/技/回復/素早さ/冒険勘/疲労などを小さく更新
   - 成長処理は `party_runtime.c` 側の bank 7 に置き、Bank 0 への追加を最小化

3. つよさ画面に独自成長値を表示
   - 戦意
   - 集中
   - 疲労
   - 冒険勘
   - 武器熟練

## 変更したファイル
- `main.c`
- `party_runtime.c`
- `party_runtime.h`

## 守った方針
- 新規BGタイル追加なし
- 新規OBJ kind追加なし
- マップ/VRAMロード順変更なし
- 32x32敵スプライト修正は維持
- 戦闘中の全画面再描画は追加していない
- 追加ロジックは主に `party_runtime.c` bank 7 へ配置

## 未確認
- この環境では GBDK コンパイル未実施
- Bank 0 overflow の有無はユーザー環境で `romusage` 確認が必要
- 成長発生時の詳細メッセージは未実装。まず内部反映とステータス表示を優先

## VBA確認ポイント
- 通常攻撃を数回行い、戦闘後に「つよさ」で熟練が伸びるか
- 戦意/集中/疲労/冒険勘が表示されるか
- 戦闘勝利後に白画面化しないか
- 32x32敵スプライトの欠け修正が維持されているか
- romusageでBank 0 overflowが出ないか
