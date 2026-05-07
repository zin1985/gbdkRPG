# 32x32 enemy sprite update

## 変更内容
- 戦闘用の敵スプライト `battle_enemy_tiles` を追加
- ユーザー提供画像を元に、以下3種を 32x32 / Game Boy 2bpp に変換
  - sprite_kind 0: スライム
  - sprite_kind 1: こうもり
  - sprite_kind 2: けもの
- 戦闘中のみ `SPRITES_8x16` を使用し、3体同時に 32x32 表示できるよう変更
- パーティ表示アイコンは 8x16 戦闘モードに合わせてタイル並びを再構成
- 戦闘カーソルも 8x16 用に `8x8 arrow + blank` の2タイル構成へ変更

## 主な修正ファイル
- `sprites.c`
- `sprites.h`
- `main.c`

## この環境で未確認のこと
- GBDK コンパイル
- 実機 / エミュレータでの見た目確認

## 補足
- フィールド用 `enemy_tiles` はそのまま残し、戦闘用だけ別配列に分離しています。
- 戦闘時だけ 8x16 OBJ モードへ切り替え、戦闘終了後は `init_map_sprites()` で 8x8 に戻る想定です。
