# rpg132_bank0_trim_world_features

Bank 0 overflow 対策版。

rpg130/rpg131 で追加した世界拡張機能を維持しつつ、main.c に残っていたマップイベント判定などを `field_feature_runtime.c` に逃がした版です。

## 主な維持機能
- 町
- 港町
- 洞窟ダンジョン
- 古代遺跡
- ダンジョン用マップチップ
- 宝箱取得/空箱判定
- 罠床ダメージ
- エリア別ランダムエンカウント率
- `QUEST_LOST_KEY` 簡易進行
- 敵3体32x32のBG表示方式

## 主な変更
- 出力ROM名：`rpg132_bank0_trim_world_features.gb`
- 追加ファイル：
  - `field_feature_runtime.c`
  - `field_feature_runtime.h`
- `build.sh` / `Makefile` の出力名も変更済み。
