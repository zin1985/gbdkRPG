# rpg230_battle_status_bank23_trim

## 目的
rpg229でまだ `Possible overflow from Bank 0 into Bank 1` が残ったため、Bank0末尾の安全域をさらに作る。

## 修正
- `battle_status_ui_runtime.c/.h` を追加
- Bank23へ以下を移動
  - 戦闘背景32x32クリア
  - パーティステータス枠描画
  - HP/MP数値表示
- `main.c` から以下を削除
  - `u16_to_dec3()`
  - `put_u16()`
  - `battle_clear_bg_full()`
  - `draw_battle_party_member_status()`
  - `draw_party_status_box()`

## 確認
- `./build.sh`
- Multiple write / Possible overflow が消えること
- 戦闘突入時、上部HP/MP表示が崩れないこと
- HP/MP変動後にステータス表示が更新されること
