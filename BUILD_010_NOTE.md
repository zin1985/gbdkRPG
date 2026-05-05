# rpg010 rollback area tile switch

009で白画面化したため、010では008で動作確認できた安全構成へ戻す。

## 基準

- rpg008 safe sharp mountains
- 008はユーザー確認で白画面化せず動作

## 戻した/入れないもの

- 009の町/フィールド別BGタイル差し替え処理は入れない
- field_map_gfx_tiles / town_map_gfx_tiles は使わない
- エリア移動時に set_bkg_data() を呼び直さない
- 新規object kindは追加しない
- kind == 4 は使わない
- 新規actor/NPCは追加しない
- MAP_GFX_TILE_COUNTは13uのまま

## 010で維持するもの

- kind 2 = 通れない尖った山の見た目
- 通行判定は collision16_map のまま
- 町マップ別配列は既存の安定範囲のみ維持
- build設定は -msm83:gb -Wl-yt0x19 -Wl-yo8

## 次の切り分け

010で動作するなら、009の白画面化原因はエリア別BGタイル差し替え処理の可能性が高い。
なだらかな山を再導入する場合は、set_bkg_data()再ロードなし・新規kindなしで行う。
