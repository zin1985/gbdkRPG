# rpg226_magic_skill_split_buildfix

## 目的
rpg225 のビルド失敗 `map_data_bank.c:253 syntax error` を修正し、次の戦闘拡張を追加する。

## 修正
- `map_data_bank.c` の `field_east_collision16_map` / `field_east_object16_map` の行末カンマ漏れを修正。
- 戦闘コマンドを6項目化。
  - こうげき / まほう
  - とくぎ / どうぐ
  - ぼうぎょ / にげる
- 特技リストと魔法リストを分離。
  - 特技：武器攻撃で閃く技のみ。
  - 魔法：カタカナ英語名の魔法系のみ。
- `とくぎ`表示パラメータを `まりょく` 表示へ変更。
- 魔法熟練度 `magic_mastery` を追加。
- 特技威力は攻撃力ベースへ変更。
- 魔法威力は魔力 + 魔法熟練度ベースへ変更。
- 魔法を使った時に魔法熟練度が上がり、魔法を閃く処理を追加。
- 魔法を閃いた場合、戦闘中に即時発動し、以後一覧に追加される。

## 注意
- こちらの環境では GBDK `lcc` が無いため実ビルド未確認。
- 次回ローカルで `./build.sh` を実行し、Bank0 overflow / Multiple write / Undefined Global を確認すること。
- `magic_mastery` を保存データ構造にも追加しているため、古いSRAMとの互換性確認が必要。
