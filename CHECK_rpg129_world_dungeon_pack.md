# CHECK rpg129_world_dungeon_pack

## 変更ファイル単位

### 修正あり

- `main.c`
  - エリアID、イベントID、宝箱フラグを追加
  - 港町・洞窟・古代遺跡への遷移を追加
  - 洞窟/遺跡のランダムエンカウント率を追加
  - 罠床・宝箱・遺跡メッセージのイベント処理を追加
  - 町系エリア判定、危険エリア判定を追加

- `sprites.h`
  - `MAP_GFX_TILE_COUNT` を 29 に変更
  - ダンジョン壁/罠床/宝箱チップの説明を追加

- `sprites.c`
  - ダンジョン用マップチップ12タイルを追加

- `map_data.h`
  - 港町・洞窟・古代遺跡のエリアIDを追加

- `map_data_bank.c`
  - 港町・洞窟・古代遺跡のcollision/object mapを追加
  - フィールド上の入口マーカーを追加

- `map_data_runtime.c`
  - area IDに応じた追加マップ参照を追加

- `messages.h`
  - 追加メッセージIDを定義

- `messages_bank.c`
  - 追加メッセージ本文とmessage table参照を追加

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

## Bank 0観点

- 大量データはbanked側へ追加。
- `main.c` には遷移・イベント分岐を最小限追加。
- Bank 0が本当に増えすぎていないかは `romusage` で確認すること。

## ビルド確認コマンド

```sh
chmod +x build.sh
./build.sh
```

## 要確認ログ

- `Possible overflow beyond Bank 0`
- `Write from one bank spans into the next`
- `ASlink-Warning-Undefined Global`
- `MAP_GFX_TILE_COUNT` 周辺のVRAM表示崩れ

