# rpg075_fix

## 目的
rpg073の戦闘初回UI安定化方針を実コード側に寄せ、戦闘画面の左上パーティ欄を2枠レイアウトへ拡張する。

## 実装内容

<details>
<summary>戦闘UI</summary>

- `draw_party_status_box()` を追加。
- 左上に `ゆうしゃ` と `そうりょ` の2枠を表示。
- `そうりょ` は表示だけの仮枠で、戦闘参加処理・仲間配列・新規actorは未追加。
- `draw_battle_frame()` は full BG clear → party box → enemy name box の順に整理。
- `battle_reset_screen_for_command()` を追加し、戦闘開始メッセージ後の初回コマンド画面を再構築。

</details>

<details>
<summary>敵名文字化け対策</summary>

- `BATTLE_NAME_LEN` を `18u` に拡張。
- `battle_data_copy_name_bank()` をUTF-8境界を意識したコピーに変更。
- 日本語敵名が途中のbyteで切れにくいようにした。

</details>

<details>
<summary>安全制約</summary>

- 新規BGタイル追加なし。
- `MAP_GFX_TILE_COUNT` 変更なし。
- 新規actor/NPC追加なし。
- 新規object kind追加なし。
- VRAMロード経路の大幅変更なし。
- `jpfont.c` / `misakiUTF16.c` のBANKED化なし。

</details>
