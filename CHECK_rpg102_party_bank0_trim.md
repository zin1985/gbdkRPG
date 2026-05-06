# CHECK rpg102_party_bank0_trim

## 目的
rpg100_party_roster_fix の Bank 0 overflow を解消する。

## 修正内容
- `party_runtime.c/h` を追加
- `party_roster[]` / active slot / reserve member / 生存判定 / 敵攻撃対象選択を bank 7 へ分離
- `main.c` から大きい PartyMember 実装ブロックを削除
- `main.c` は表示・戦闘進行に必要な party API だけ呼ぶ
- rpg101 のフィールドスプライト差し替えは維持

## 維持しているもの
- そうりょ / まほう の仲間データ
- 予備仲間3名
- 敵攻撃が仲間からランダム対象選択する処理
- 全員HP0で敗北
- ゆうしゃとまほうの戦闘アイコン交換
- 12x4シート由来のフィールドプレイヤースプライト

## 確認観点
- romusage の Bank 0 overflow warning が消えるか
- rpg100で出ていた `_HOME 0x4000 -> 0x43f2` が改善するか
- 戦闘UIが崩れていないか
- 仲間名が化けないか
- 敵攻撃で仲間HPが減るか
- フィールドプレイヤースプライトが rpg101 と同じく差し替わっているか
