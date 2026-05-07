# UI Frame Sample 3-tone (rpg118)

- ユーザー提供サンプル3点（角 / 囲みイメージ / 横線）を元に UI 枠タイルを差し替え。
- 角は 8x8 サンプルから 90 度回転で UL/UR/LR/LL を生成。
- 横線は 8x8 サンプルから 90 度回転で TOP/RIGHT/BOTTOM/LEFT を生成。
- 色は Game Boy 4階調のうち 3色を使用。
  - color0: 白
  - color2: 中間グレー
  - color3: 黒
- 交差点タイルは作らず、ウインドウの重なりで見せる方針を維持。
- 変更対象は `jpfont.c` の共通UI枠タイルと、`jp_window_prepare()` / `draw_bkg_box()` の辺選択処理。
