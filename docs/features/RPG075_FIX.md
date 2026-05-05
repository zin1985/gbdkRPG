# rpg075_fix

<details open>
<summary>概要</summary>

rpg074_ui のビルドエラー修正版です。

`main.c:1689: error 12: called object is not a function` の原因は、
`battle_reset_screen_for_command(UINT8 display_off)` の引数名 `display_off` が、
GBDK側の `DISPLAY_OFF` マクロ展開で呼ばれる `display_off()` と衝突していたためです。

rpg075_fix では引数名を `turn_display_off` に変更し、マクロ衝突を避けました。

</details>

<details>
<summary>修正内容</summary>

- `battle_reset_screen_for_command(UINT8 display_off)` を `battle_reset_screen_for_command(UINT8 turn_display_off)` に変更。
- 関数内の `if (display_off)` を `if (turn_display_off)` に変更。
- ビルド出力名を `rpg075_fix.gb` に変更。
- rpg074で追加した左上2人分パーティUI、敵名長18、初回コマンド前再描画処理は維持。

</details>

<details>
<summary>修正していない箇所</summary>

- 新規BGタイル追加なし。
- `MAP_GFX_TILE_COUNT` 変更なし。
- 新規actor/NPC追加なし。
- 新規object kind追加なし。
- VRAMロード経路変更なし。
- `jpfont.c` のBANKED化なし。
- `misakiUTF16.c` のBANKED化なし。

</details>
