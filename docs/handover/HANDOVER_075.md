# HANDOVER rpg075_fix

<details open>
<summary>今回の位置づけ</summary>

rpg075_fix は rpg074_ui のビルドエラー修正版です。

rpg074_ui で `main.c:1689: error 12: called object is not a function` が発生しました。
原因は `battle_reset_screen_for_command(UINT8 display_off)` の引数名が、GBDKの `DISPLAY_OFF` マクロ展開で使われる `display_off()` と衝突したことです。

</details>

<details>
<summary>次回候補</summary>

次は `rpg076_party.zip` 候補です。

- 表示だけの `そうりょ` を仮パーティデータ構造に分離。
- まだ戦闘参加はさせない。
- actor/NPC/OAM追加はしない。

</details>
