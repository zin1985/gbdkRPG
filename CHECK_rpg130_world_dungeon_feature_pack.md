# CHECK rpg130_world_dungeon_feature_pack

## 目的

rpg128_enemy_bg_tiles をベースに、敵32x32 BG表示を維持したまま、町・港町・洞窟・古代遺跡を追加して世界を広げる版。

## 実装した内容

### 追加エリア

- 既存町
- 港町
- 洞窟ダンジョン
- 古代遺跡

### 追加マップ/マップチップ

- `map_data_bank.c` に以下の16x16マップを追加
  - `port_collision16_map` / `port_object16_map`
  - `dungeon_collision16_map` / `dungeon_object16_map`
  - `ruins_collision16_map` / `ruins_object16_map`
- `sprites.c` にダンジョン系BGチップを追加
  - dungeon wall 4 tiles
  - dungeon pit/hazard 4 tiles
  - chest 4 tiles

### 追加イベント

- フィールドから港町へ遷移
- フィールドから洞窟へ遷移
- フィールドから古代遺跡へ遷移
- 各追加エリアからフィールドへ戻る
- 洞窟/遺跡の宝箱取得
- 2回目以降の宝箱空表示
- 洞窟/遺跡の罠床小ダメージ
- 古代遺跡の石碑風メッセージ

### 検討中機能の軽量実装

- エリア別ランダムエンカウント率
- 危険エリア判定
- 町系エリア判定
- 宝箱フラグ
- `QUEST_LOST_KEY` の開始/進行
- 3体32x32敵のBG表示維持

## 修正ファイル単位

### 修正あり

- `main.c`
- `sprites.h`
- `sprites.c`
- `map_data.h`
- `map_data_bank.c`
- `map_data_runtime.c`
- `messages.h`
- `messages_bank.c`
- `build.sh`

### 修正なし

- `battle_data_bank.c`
- `battle_data_runtime.c`
- `battle_text.c`
- `inventory.c`
- `party_runtime.c`
- `actor_runtime.c`
- `audio.c`
- `jpfont.c`
- `misakiUTF16.c`

## Bank 0保護方針

- 大量データはbanked側へ追加。
  - マップ: `map_data_bank.c`
  - メッセージ: `messages_bank.c`
  - マップチップ: `sprites.c` bank 3
- `main.c` にはエリア遷移・イベントルーティングのみ追加。
- 正確なBank 0増減は、ユーザー環境で `./build.sh` 実行後の `romusage` を確認すること。

## 要確認

- `Possible overflow beyond Bank 0`
- `Write from one bank spans into the next`
- `ASlink-Warning-Undefined Global`
- `MAP_GFX_TILE_COUNT` 変更に伴う表示崩れ

## 確認コマンド

```sh
chmod +x build.sh
./build.sh
```
