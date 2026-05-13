# rpg234_equip_status_skill_fix 確認メモ

## 修正目的

- 装備メニューを開くとフリーズする問題の軽減/修正
- ステータス3枚目以降で、ひらめいた特技を汎用名ではなく実際の特技名で表示
- 閃き時にBanked文字列ポインタをそのまま返して表示する危険を回避
- 特技/魔法を少し追加

## 主な変更

### party_runtime.c

- 装備候補配列 `item_choices[48]` をローカルスタックから静的WRAM配列 `party_equip_choices[]` へ移動
  - Banked関数内の大きなローカル配列によるスタック圧迫を避ける
- 装備画面開始時に `HIDE_WIN` / `SHOW_BKG` / `SPRITES_8x8` を明示
- ステータスページ数を 3 から 10 へ拡張
- 3ページ目以降を「とくぎ一覧」ページとして扱い、仲間ごとの `learned_skills[]` から実際の特技名を表示
- 魔法は特技一覧から除外し、魔法メニュー側に残す

### battle_skill_runtime.c/.h

- `battle_skill_runtime_name_buffered()` を追加
  - Banked ROM内の文字列ポインタをそのままmain側に返す危険を避けるため、WRAMバッファへコピーしてから返す
- `battle_skill_runtime_put_name()` を追加
  - 他Bankから技名を表示したい場合に、Bank5内で直接 `jp_put_bkg_text()` する

### main.c

- 閃き時の技名/魔法名表示を `battle_skill_runtime_name_buffered()` 経由に変更

### skill_defs.h / battle_skill_meta_runtime.c

- SKILL_MAXを81へ拡張
- 追加特技/魔法：
  - スカイ
  - ヴォイド
  - アルカナ
  - コメット矢
  - ゴッド手
  - ゼロ装置
  - ゼロ
  - ルナ
  - フォトン
  - アルテマ

## ローカル確認

```sh
./build.sh
```

確認項目：

- 装備を開いてもフリーズしない
- 装備候補を開く/閉じる/上下移動できる
- 装備変更後に戻れる
- ステータス3枚目以降で、覚えた特技名が実名表示される
- 閃き時に「ひらめいた!」の後、技名が正常表示される
- `Multiple write` / `Possible overflow from Bank 0 into Bank 1` が出ない
