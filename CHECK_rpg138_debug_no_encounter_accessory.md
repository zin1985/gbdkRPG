# rpg138_debug_no_encounter_accessory チェック項目

## 目的
デバッグ用アクセサリー「退魔のすず」を装備している間、フィールド・洞窟・古代遺跡のランダムエンカウントを0にする。

## 変更点
- `ITEM_DEBUG_NO_ENCOUNT` を追加。
- 初期所持品に「退魔のすず」を1個追加。
- 装飾品として装備可能。
- `party_debug_no_encounter_accessory_equipped()` を追加。
- `field_feature_encounter_rate()` 側で装備中ならエンカウント率を0にする。
- Bank 0側の `field_random_encounter_should_start()` には新規ロジックを追加せず、既存のbanked呼び出し先で判定する。

## 確認ポイント
1. ビルド名が `rpg138_debug_no_encounter_accessory.gb` になっている。
2. Bank 0 overflow が出ない。
3. メニューの装備画面で「退魔のすず」が装飾品候補に出る。
4. 誰かが「退魔のすず」を装備している間、フィールド・洞窟・古代遺跡でランダムエンカウントしない。
5. 外すと通常のエンカウント率に戻る。
