# rpg223_bank_guard_static_review

## 対応内容

rpg222 を引き継ぎ、まず実ビルド前の安全確認と、危険警告の見落とし防止を強化した。

今回の環境では `/opt/gbdk/bin/lcc` が存在しないため、GBDK実ビルドは未実施。  
ただし、ファイル構成・ビルド対象・Bank分割・最新引き継ぎに沿った静的確認を行った。

## 変更ファイル

### `bank_ids.h`

`ROM_BANK_FIELD_MAP_RENDER_CONCRETE 19u` が include guard の `#endif` の外側に出ていたため、ガード内へ移動。

機能上ただちに壊れる形ではないが、今後複数回 include された場合に定義位置が不自然で、Bank定義の保守性が悪くなるため修正。

### `build.sh`

`lcc` が見つからない場合に、通常の shell エラーではなく、明示的に以下を表示して終了するようにした。

```text
[ERROR] lcc not found. Set /opt/gbdk/bin in PATH or install GBDK before building.
```

また、危険警告検出用の補助スクリプトを呼び出す処理を追加。

### `Makefile`

`GBDK_PATH` / `lcc` が無い場合に明示エラー化。  
また、補助スクリプト `tools/check_build_log_strict.py` を呼び出すようにした。

### `tools/check_build_log_strict.py`

`.gb` が生成されても失敗扱いにすべき危険警告を検出する補助ツールを追加。

検出対象:

- `Possible overflow from Bank 0 into Bank 1`
- `Possible overflow beyond Bank 0`
- `Multiple write`
- `Write from one bank spans into the next`
- `ASlink-Warning`
- `Undefined Global`
- `Bank 19 overflow`
- `Bank 0 overflow`
- `relocation truncated`
- `segment overlap`

### `tools/rpg_bank_guard.md`

補助ツールの使い方メモを追加。

### `docs/design/15_rpg223_build_guard_and_static_review.md`

今回の静的確認結果と、次にローカルGBDK環境で確認すべき内容を整理。

## 静的確認結果

### build.sh / Makefile

`field_map_render_runtime.c` はビルド対象に含まれている。

### Bank 19

`field_map_render_runtime.c` は `#pragma bank 19` 指定あり。

### Bank 0削減方針

`main.c` 側の `draw_object_map()` は薄いbridgeのみ。

```c
static void draw_object_map(void) {
    field_map_render_runtime_draw(current_area);
}
```

### 既知の未確認点

GBDKの `lcc` が無いため、以下は未確認。

- Bank0 overflowの有無
- Bank19 overflowの有無
- Undefined Globalの有無
- 実ROM起動確認
- アイティル塔UI後のキャラ復帰確認

## 次にやること

ローカルGBDK環境で以下を実行。

```sh
./build.sh
python3 tools/check_build_log_strict.py build.log compile_output.log romusage_output.log
```

危険警告が出た場合は、GBファイルが生成されても失敗扱い。

## 注意

今回の rpg223 は、ゲーム内容を大きく増やす前に、rpg222 のBank分割を安全に検証するためのガード強化版。  
次の実装では、街・ダンジョン・イベント追加を `map_event_runtime.c` / `field_map_render_runtime.c` / Banked data 側に寄せる。
