# rpg223 Bank Guard 補助ツール

`tools/check_build_log_strict.py` は、GBDK のビルドが `.gb` を出力しても失敗扱いにすべき危険警告を検出するための補助ツールです。

確認対象:

- `Possible overflow from Bank 0 into Bank 1`
- `Possible overflow beyond Bank 0`
- `Multiple write`
- `Write from one bank spans into the next`
- `ASlink-Warning`
- `Undefined Global`
- `Bank 19 overflow`
- `Bank 0 overflow`

使い方:

```sh
python3 tools/check_build_log_strict.py build.log compile_output.log romusage_output.log
```

このプロジェクトでは、GBファイルが生成されても上記が出た場合は失敗扱いにします。
