# rpg114_saga_system_pack_bank0_guard

## 目的
rpg113で動作確認済みのBank 0安全版を基準に、これまで検討した将来機能を「Bank 0を増やさない」方針で可能な限り実装へ寄せる。

## 絶対方針
- main.cは変更しない。
- 新しいBank 0呼び出し口を増やさない。
- 既存のparty_runtime.c内部だけで効く派生ステータス・熟練・習得フラグを追加する。
- UI、メニュー、セーブ形式、新規イベントVMは今回は未実装。

## 追加した実装土台

### 1. 種族フラグ
party_runtime.c内部に以下の種族分類を追加。

- HUMAN
- MUTANT
- MECH
- MONSTER

現時点では、まほうをMUTANT相当に設定し、魔法系能力に補正が入る。
MECH/MONSTERは将来用の予約。

### 2. LP土台
各パーティメンバーに max_lp / lp を追加。
HPが0になった時にLPが1減る。
現時点ではUI表示やゲームオーバー処理には接続しない。

### 3. 装備重量・陣形ロール
各メンバーに equip_weight / formation_role を追加。
party_get_active_fighter() の返す派生ステータスに反映する。

- BACK: 弓以外の攻撃を少し抑制
- SWIFT: 素早さ補正
- GUARD: 防御補正
- weight: 素早さ低下

### 4. 武器熟練の段階化
既存の weapon_mastery を拡張し、一定値で内部フラグを付与。

- 6以上: RANK1
- 16以上: RANK2
- 28以上: SPARK予約
- 杖10以上: 回復魔法フラグ

### 5. 技・魔法習得フラグ
learned_tech_flags / learned_magic_flags を利用。
UI表示は未実装だが、派生ステータスやMP消費軽減に効く。

### 6. 派生ステータス
party_get_active_fighter() で返す attack/defense/skill/heal/agility を、内部状態から算出するよう変更。
main.c側は既存APIを呼ぶだけなので、Bank 0側の呼び出しは増えない。

### 7. 回復時の状態異常解除土台
回復行動時に SLEEP / STUN を解除する土台を追加。
毒などのターン処理は未実装。

## 今回あえて未実装にしたもの
- 新規メニューUI
- 技一覧UI
- LP表示
- 状態異常アイコン
- 敵ランクテーブル本体
- ボスAIテーブル
- Event VM
- SaveData拡張
- 5000x5000チャンクロード

## 理由
これらはmain.cやUI、セーブ形式、ビルド構成へ影響しやすいため、Bank 0安全確認後に別バージョンで進める。

## 動作確認ポイント
- rpg113同様に起動すること
- 戦闘に入れること
- ゆうしゃ、そうりょ、まほうの手動操作が維持されていること
- 特技/回復使用でMPが消費されること
- 成長表示が壊れていないこと
- romusageでBank 0 overflowが出ないこと

## 次の安全な実装候補
1. rpg114のBank 0確認
2. 動作確認
3. party_runtime.c内のLP/熟練/魔法フラグを表示する小型デバッグメッセージ
4. ただしUI拡張前にBank 0余白を確認する
