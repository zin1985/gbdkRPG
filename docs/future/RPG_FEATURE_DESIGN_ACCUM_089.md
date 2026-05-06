# RPG_FEATURE_DESIGN_ACCUM_089

## 反映した検討内容

rpg089_skill_slots では、これまでのスレッド検討のうち、現時点で安全に実装しやすい内部構造だけを反映した。

### 1. SkillDef拡張

SkillDefを、将来の複数技・状態異常・AI利用へ拡張しやすい形にした。

```c
typedef struct SkillDef {
    UINT8 id;
    UINT8 kind;
    UINT8 target;
    UINT8 mp_cost;
    UINT8 power;
    UINT8 accuracy;
    UINT8 flags;
} SkillDef;
```

追加した考え方:

- id: 技ID
- kind: ダメージ/回復/デバフなど
- target: 敵/自分/全体など
- mp_cost: 消費MP
- power: 効果量補正
- accuracy: 命中率
- flags: フィールド使用可、AI使用可、防御無視などの将来拡張用

### 2. 固定4スロット方式

技一覧UIを追加する前段階として、固定4スロットの習得技構造を追加した。

```c
#define PLAYER_SKILL_SLOT_COUNT 4u

typedef struct PlayerSkillSet {
    UINT8 slots[PLAYER_SKILL_SLOT_COUNT];
} PlayerSkillSet;
```

現時点では、既存の戦闘メニューを変えずに以下の対応とする。

- とくぎ: slot 0
- かいふく: slot 1
- slot 2/3: 将来用

### 3. 既存UIを変えない移行

まだ技一覧ウィンドウや対象選択UIは追加しない。

理由:

- 戦闘UIの位置ズレがまだ繊細
- Bank 0に余裕が少ない
- 技一覧UIは入力状態管理・カーソル・VRAM更新が増える

そのため、今回の実装では見た目を変えず、内部処理だけを `player_use_skill(skill_id)` に集約した。

### 4. 次回以降の拡張候補

- 技一覧UI
- BattleState の細分化
- pressed入力管理
- 対象選択UI
- 状態異常
- 技習得テーブル
- セーブデータへの skill slot 保存

## 未実装のまま残したもの

- 技一覧UI
- 技習得処理
- レベルアップ
- 状態異常
- 対象選択
- 敵AIのSkillDef利用
- SkillDefのbanked data化

## 実装方針メモ

次に進める場合は、まず `player_learn_skill()` と `LearnSkillEntry` を内部実装し、UIを増やさずにスロットへ技を追加できるようにする。
その後、BattleStateを細分化してから技一覧UIへ進む。
