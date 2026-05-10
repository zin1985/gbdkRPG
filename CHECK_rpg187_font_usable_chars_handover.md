# rpg187_font_usable_chars_handover

## 対応内容

- `rpg186_official_misaki_rpg_kanji` をベースに、使用可能文字一覧をドキュメント化。
- 漢字一覧を以下に分離して整理。
  - もともとの教育版ベース漢字: 1007字
  - rpg185/rpg186で追加したRPG頻出漢字: 88字
- 追加ファイル:
  - `FONT_USABLE_CHARS_rpg187.md`
  - `FONT_USABLE_CHARS_rpg187.txt`
- `README.md` の引継ぎ情報末尾に rpg187 セクションを追記。

## コード変更

なし。

## Bank影響

- `main.c` 未変更
- フォントデータ未変更
- Bank 0増加なし
- Bank 6増加なし

## 注意

- この版はドキュメント追加のみ。
- 現行テーブルには `℃№℡Å`、ローマ数字、丸数字、数学記号などが残っている。不要なら次回削減候補。
