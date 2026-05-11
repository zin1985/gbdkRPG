# rpg202_itil_tower_quiz

## 追加内容
- 100階ある「ITILの塔」を追加。
- フィールド下部中央 `(8,14)` から塔へ入る。
- 入場時に中央UIで `ITILの塔◯階` を表示。
- 各階に3択ITIL問題NPCを配置。
  - NPC位置: `(8,8)`
  - 正解すると右出口へ進める。
  - 不正解なら即退場。
- 各階の右出口を実装。
  - 右出口位置: `(14,2)`
  - 100階未満: 次の階へ進む。
  - 100階: ランダム報酬を獲得して退場。
- ランダム報酬を追加。
  - `ITILの剣`: 攻撃力 +100
  - `ITILの鎧`: 防御力 +100
  - `ITILの紋章`: とくぎ +100

## 実装方針
- Bank 0を大きく増やさないため、問題UI・階層状態・報酬付与は `itil_tower_runtime.c` に分離。
- `itil_tower_runtime.c` は Bank 10。
- 100問をベタ持ちせず、12問を階層ごとにローテーションする方式。
- マップは既存ダンジョンマップを塔エリアでも流用し、専用大規模マップ配列は追加しない。
- `MAP_GFX_TILE_COUNT` は増やしていない。

## 追加/変更ファイル
- `itil_tower_runtime.c`
- `itil_tower_runtime.h`
- `main.c`
- `field_feature_runtime.c/h`
- `map_data.h`
- `map_data_runtime.c`
- `map_data_bank.c`
- `actor_runtime.c`
- `inventory.h/c`
- `party_runtime.c`
- `ui_icons.c`
- `save_bridge_runtime.c`
- `build.sh`
- `bank_ids.h`

## 注意
- この環境ではGBDK実ビルド未実行。
- Bank 10にクイズ処理を追加したため、ビルド後は `romusage` でBank 10 overflowを確認すること。
- Bank 8は引き続き新規配置先として使わない。
