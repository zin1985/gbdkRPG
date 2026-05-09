# rpg160_battle_party_name_enemy_y_fix

## 修正内容

- 敵キャラBG表示の基準Y座標を `BATTLE_ENEMY_BG_Y=6u` に変更し、rpg159から2段下げた。
- 味方パラメータ上段の名前表示を `party_put_active_name_battle()` 経由に変更。
  - `party_get_active_name()` はBank 7側文字列ポインタを返すため、Bank 0側で後から描画すると空白・化けの原因になる。
  - Bank 7が有効な状態で `jp_put_bkg_text()` を呼ぶBANKEDヘルパーを追加した。
- 上部HP/MP枠は1つの連続ウィンドウのまま、内部3行を明示的にクリアしてから名前・HP・MPを描画する。

## 未確認

- この環境ではlccによるROM実ビルドは未実施。
