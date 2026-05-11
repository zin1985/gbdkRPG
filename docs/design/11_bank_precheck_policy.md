# Bank重なり・Overflowの事前チェック方針

## 結論

完全なBank重なり・Overflow判定は、**コンパイル前だけでは確定できない**。

理由は、GBDK/SDCCではCソースのサイズと、最終的に各ROM Bankへ配置される `_CODE_N` のサイズが一致しないため。  
関数展開、定数、呼び出しスタブ、BANKED呼び出し、最適化、リンク順で最終サイズが変わる。

ただし、コンパイル前に以下はチェックできる。

- `#pragma bank N` の配置一覧
- `build.sh` の対象 `.c` 一覧
- `bank_ids.h` の定義と `#pragma bank` のズレ
- 危険Bankへの配置
- 過去に重なりが出たBank 8の使用
- Cソースサイズ合計から見た危険度
- 前回 `.map` がある場合の、Bank使用量の事前参考チェック

## 追加したスクリプト

```bash
python3 tools/prebank_static_check.py
python3 tools/prebank_static_check.py --strict
python3 tools/prebank_static_check.py --map rpg198_bank8_overlap_actor_bank1_fix.map
```

## 判定の使い分け

### コンパイル前にできること

`tools/prebank_static_check.py` で、以下を確認する。

- Bank 8に `actor_runtime.c` / `inventory.c` などを置いていないか
- Bank番号が `-Wl-yo16` の範囲を超えていないか
- 1つのBankに巨大なCファイルが集中していないか
- `bank_ids.h` 上の具体Bank指定が不自然でないか

### コンパイル後に必ず見ること

最終判定は必ず以下で行う。

```bash
./build.sh
/opt/gbdk/bin/romusage rpgXXX.map -a -g -B
/opt/gbdk/bin/romusage rpgXXX.map -q -E
```

危険ワード:

```text
Write from one bank spans into the next
Multiple write
Possible overflow
Overflow by
Area _CODE_N extends past end of memory region
```

これらが出た場合は、GBファイルが生成されても実機投入禁止。

## 今回までの知見

### Bank 0

- かなり逼迫している。
- main.cへ大型処理・長いswitch・文字列・テーブルを追加しない。
- `main.c bytes` は目安であり、最終判定はROM_0使用率を見る。

### Bank 7

- `party_runtime.c` が大きく、過去に `_CODE_7 overflow` が出た。
- rpg194で `actor_runtime.c` をBank 7から逃がした。
- 今後もBank 7には大型機能を追加しない。

### Bank 8

- 過去に `0x20000` 起点の `Multiple write` と `Possible overflow from Bank 0 into Bank 8` が複数回出た。
- `inventory.c` / `actor_runtime.c` をBank 8に置くと危険だった。
- 当面、Bank 8は新規配置先として使わない。

### 安全寄りの逃がし先

- `ui_icons.c` はBank 1へ退避済み。
- `inventory.c` はBank 11へ退避済み。
- `actor_runtime.c` はBank 1へ退避済み。
- `battle_data_bank.c` / `game_flags.c` / `quest.c` はBank 10側へ退避済み。

## 引き継ぎ時の注意

次スレッドでは、ビルド前にまず以下を実行する。

```bash
python3 tools/prebank_static_check.py --strict
```

その後ビルドし、romusageで最終判定する。

ビルドが成功しても、romusageでOverflowが出ている場合は失敗扱いにする。  
特に「白画面」は、コンパイル成功後にBank overflowが残っていた時に起きやすい。
