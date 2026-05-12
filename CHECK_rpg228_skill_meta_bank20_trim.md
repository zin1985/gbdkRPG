# rpg228_skill_meta_bank20_trim

## 目的
rpg227 で発生した以下の危険警告を解消するため、main.c に残っていた特技/魔法メタデータと威力計算を Banked runtime に分離した。

- Multiple write
- Possible overflow from Bank 0 into Bank 1

## 原因
rpg227 では、魔法/特技分離の実装により main.c に以下が残っていた。

- 50個規模の skill_table
- SkillDef 構造体
- skill_get_def()
- calc_skill_damage()
- calc_heal_amount()
- skill_is_magic()
- skill_is_heal_magic()

これにより main.c が肥大化し、Bank0 が再び危険域に入った。

## 修正内容

### Bank0から削除
- skill_table
- SkillDef
- skill_get_def()
- calc_skill_damage()
- calc_heal_amount()
- skill_is_magic()
- skill_is_heal_magic()

### 追加
- battle_skill_meta_runtime.c
  - Bank20
  - 特技/魔法メタテーブル
  - MPコスト取得
  - 種別取得
  - 魔法判定
  - 回復魔法判定
  - 特技/魔法ダメージ計算
  - 回復量計算

### 更新
- battle_skill_runtime.h
  - Banked APIを追加
- battle_skill_runtime.c
  - UI側の魔法/特技判定をBank20 APIへ寄せた
- build.sh / Makefile
  - battle_skill_meta_runtime.c をビルド対象へ追加
  - 出力名を rpg228_skill_meta_bank20_trim.gb に変更
- bank_ids.h
  - ROM_BANK_SKILL_META_CONCRETE 20u を追加

## Bank0削減結果
prebank_check.py の main.c bytes:

- rpg227: 82336 bytes
- rpg228: 75633 bytes

約 6.7KB 削減。

## 要確認
ローカルで ./build.sh を実行し、以下が出ないことを確認する。

- Multiple write
- Possible overflow from Bank 0 into Bank 1
- Write from one bank spans into the next
- Bank20 overflow
- Undefined Global
- 白画面化

