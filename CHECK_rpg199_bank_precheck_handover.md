# rpg199_bank_precheck_handover

## 追加内容

Bank重なり・Overflowの事前チェック方針を追加。

## 回答要点

- コンパイル前だけでは、最終的なBank overflow / Multiple writeは完全には確定できない。
- ただし、`#pragma bank`、`bank_ids.h`、危険Bank使用、Cソースサイズ合計、前回`.map`を使った予兆チェックは可能。
- 今回 `tools/prebank_static_check.py` を追加した。
- `docs/design/11_bank_precheck_policy.md` に引き継ぎ情報を追加した。
- `README.md` にrpg199追記を追加した。

## 推奨手順

```bash
python3 tools/prebank_static_check.py --strict
./build.sh
/opt/gbdk/bin/romusage rpgXXX.map -a -g -B
/opt/gbdk/bin/romusage rpgXXX.map -q -E
```

## 失敗扱いにする警告

```text
Write from one bank spans into the next
Multiple write
Possible overflow
Overflow by
Area _CODE_N extends past end of memory region
```

## Bank運用メモ

- Bank 0は今後も増やさない。
- Bank 7は`party_runtime.c`中心で逼迫しやすい。
- Bank 8は`0x20000`起点のMultiple writeが出やすかったため、当面新規配置先として避ける。
- `inventory.c`はBank 11、`actor_runtime.c`はBank 1、`ui_icons.c`はBank 1に逃がす方針。
