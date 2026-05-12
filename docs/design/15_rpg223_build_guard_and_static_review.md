# rpg223 Build Guard と静的レビュー

## 目的

rpg222 で `draw_object_map()` 本体を Bank19 に分離したため、次の実装へ進む前に、以下を失敗として確実に検出できるようにする。

- Bank0 overflow
- Bank19 overflow
- Multiple write
- Write from one bank spans into the next
- Undefined Global
- ASlink-Warning

GBDKでは `.gb` が生成されても危険警告が残っている場合がある。  
このプロジェクトでは、それを成功扱いにしない。

## 今回の変更

### 1. Bank定義の整理

`bank_ids.h` の `ROM_BANK_FIELD_MAP_RENDER_CONCRETE` を include guard 内へ移動した。

### 2. build.sh / Makefile の失敗理由を明確化

GBDKが入っていない環境では、`lcc: not found` の shell エラーではなく、明示的なエラーを出すようにした。

### 3. 危険警告検出ツール追加

追加ファイル:

- `tools/check_build_log_strict.py`
- `tools/rpg_bank_guard.md`

使い方:

```sh
python3 tools/check_build_log_strict.py build.log compile_output.log romusage_output.log
```

## ローカル確認手順

```sh
cd rpg223_bank_guard_static_review
./build.sh
python3 tools/check_build_log_strict.py build.log compile_output.log romusage_output.log
```

確認ポイント:

- `rpg222_bank0_field_renderer_split.gb` が生成されること
- ただし、危険警告があれば失敗扱い
- `romusage` が使える環境では Bank0 / Bank19 の使用量を確認

## 次の開発方針

rpg222/rpg223 の安全確認が通ったら、次は以下を進める。

1. `field_feature_map_event()` の座標分岐をさらにテーブル化する
2. `inspect_map_event()` の大きいイベント分岐をBanked runtimeへ逃がす
3. 街・ダンジョン追加用の `AREA_*` / `MAP_EVENT_*` 追加ルールをドキュメント化する
4. Bank19が膨らむ場合は renderer table bank を作る

## 残す判断

以下は現時点では Bank0 に残す。

- `draw_all_actors()`
- `update_player_movement()`
- `map_input()`
- `battle_input()`
- `battle_update_dirty()`
- `restore_field_vram_state()`
- `change_area_marker()`

理由は、OAM・VBlank・main.c内 static 状態への依存が強く、無理にBanked化すると白画面リスクが高いため。
