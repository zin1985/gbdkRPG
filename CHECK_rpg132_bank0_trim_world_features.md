# rpg132_bank0_trim_world_features

## 目的
rpg130/rpg131 系で追加した町・港町・洞窟・古代遺跡・宝箱・罠床・エリア別エンカウントなどの世界拡張機能を維持しつつ、Bank 0 overflow を避けるために main.c の追加ロジックを削減する。

## 修正内容
- `field_feature_runtime.c` / `field_feature_runtime.h` を追加。
- マップイベント判定を `main.c` から bank 7 側の `field_feature_map_event()` へ移動。
- エリア別エンカウント率判定を `field_feature_encounter_rate()` へ移動。
- 宝箱フラグ選択を `field_feature_chest_flag()` へ移動。
- 町・港町・洞窟・遺跡の入退場処理を `change_area_marker()` に集約し、重複コードを削減。
- `build.sh` と `Makefile` の出力ROM名を `rpg132_bank0_trim_world_features.gb` に変更。
- ビルド対象ソースへ `field_feature_runtime.c` を追加。

## 期待効果
- `main.c bytes` が rpg130/rpg131 相当の 101KB 台から 99KB 台へ減少。
- 非バンク領域の `_HOME` 溢れを減らす狙い。
- 世界拡張機能自体は削除せず、判定ロジックをBanked側へ逃がす。

## 確認ポイント
```sh
chmod +x build.sh
./build.sh
```

確認すること：
- 出力名が `rpg132_bank0_trim_world_features.gb` になっていること。
- `romusage` で `Possible overflow beyond Bank 0` が消えるか。
- 町・港町・洞窟・古代遺跡への入退場が維持されていること。
- 宝箱・罠床・エリア別エンカウントが維持されていること。

## 注意
このChatGPT環境にはGBDK本体がないため、実ビルドは未実行。ZIP整合性のみ確認する。
