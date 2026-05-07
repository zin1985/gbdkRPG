# rpg116_equipment_items_status_bank0_guard

## 目的

rpg115 の「LPなし独自成長・熟練土台」に対して、武器熟練がゲーム上の意味を持つように、最小のアイテム・装備・表示導線を追加する。

## 実装方針

- LPは引き続き不採用。
- Bank 0を守るため、重い表示・定義・装備計算は bank 7 側へ寄せる。
- main.c はメニュー呼び出しだけを変更し、ページ本体は party_runtime.c / inventory.c 側へ逃がす。
- party_runtime.h の公開API追加はメニュー表示用の最小2本のみ。
- inventory.h のアイテムID定義を拡張し、やくそう・まほう草・どくけし・鍵・武器・防具・アクセサリを同じID体系に置く。

## 追加機能

### 1. もちもの定義

- やくそう
- まほう草
- どくけし
- しろのかぎ
- 木のけん
- 鉄のけん
- 木のつえ
- まどうのつえ
- 短いゆみ
- けいこ手甲
- 道具キット
- ぬののふく
- かわよろい
- 鉄よろい
- おまもり
- はね飾り

初期もちものは `inventory_seed_defaults()` で投入する。

### 2. 武器・防具・アクセサリ定義

`party_runtime.c` に `PartyEquipmentDef` を追加し、以下を定義する。

- item_id
- slot
- weapon_type
- attack
- defense
- skill
- heal
- agility
- weight
- element_resist
- status_resist
- name

### 3. キャラごとの装備スロット

`PartyMemberRuntime` に以下を追加する。

- weapon_id
- armor_id
- accessory_id

初期装備をキャラごとに設定する。

### 4. 装備武器から weapon_type を決定

従来の `member->weapon_type` 直接参照ではなく、装備中の weapon_id から装備定義を引き、weapon_type を決める。

### 5. 装備の attack / defense / skill / heal / agility / weight を派生ステータスへ反映

`party_get_active_fighter()` が返す派生ステータスに装備補正を含める。

### 6. 装備武器種に応じて熟練度が伸びる

`party_gain_mastery()` が装備中武器から weapon_type を取得し、その武器種の熟練を上げる。

### 7. 仲間ごとのパラメータ表示

メニューの「つよさ」で bank 7 側の `party_menu_show_status_loop()` を呼び出す。

- LEFT/RIGHT で仲間切り替え
- HP/MP
- ちから
- まもり
- とくぎ
- かいふく
- すばやさ
- 武器種
- 熟練
- 冒険勘

### 8. 仲間ごとの装備表示

メニューの「そうび」で bank 7 側の `party_menu_show_equip_loop()` を呼び出す。

- LEFT/RIGHT で仲間切り替え
- 武器
- 防具
- 装飾
- 武器種
- 攻撃補正
- 防御補正
- 重量

装備変更UIは未実装。次段階で行う。

### 9. もちもの表示

メニューの「もちもの」で bank 7 側の `inventory_menu_show_items_loop()` を呼び出す。

- 所持数が1以上のアイテムだけ表示
- 上下でカーソル移動
- 現時点では使用処理は未実装

## Bank 0対策

- `main.c` の既存直書きステータス表示を bank 7 側へ移動。
- `main.c` bytes は rpg115 より減少した。
- prebank_check 上の main.c bytes は 95608。

## 未実装

- 装備変更UI
- アイテム使用処理
- 戦闘中のやくそう使用
- 店売買
- 装備可能キャラ判定
- セーブデータへの装備・もちもの保存拡張
- 技一覧・魔法一覧UI

## 次回候補

rpg117 では以下が候補。

1. やくそう使用処理
2. 戦闘中アイテム使用
3. 装備変更UI
4. 装備可能判定
5. 店UIの最小実装
