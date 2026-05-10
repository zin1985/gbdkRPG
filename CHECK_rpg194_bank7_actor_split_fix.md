# rpg194_bank7_actor_split_fix

## 目的
rpg193は通常のリンク警告は消えていたものの、romusageで `_CODE_7` が 424 bytes overflow していました。
この状態はGBDKでは「ビルドは通るが実機では白画面化する」典型的な危険状態です。

## 対応内容
- `actor_runtime.c` を Bank 7 から Bank 8 へ移動。
- `party_runtime.c` は Bank 7 のまま維持。
- `inventory.c` は Bank 11 のまま維持。
- `ui_icons.c` は Bank 1 のまま維持。
- `bank_ids.h` に `ROM_BANK_ACTOR_RUNTIME_CONCRETE 8u` を追加。
- `build.sh` の出力名を `rpg194_bank7_actor_split_fix.gb` に変更。
- `build.sh` にビルド前クリーン処理を維持/追加し、古い中間ファイルによるBank重複を避ける。

## 維持している内容
- rpg188 UI/ページ/戦闘道具系の修正
- rpg189 Bank8重複対策
- rpg190 カーソル統一
- rpg191 港町の海・橋表現
- rpg192 ツボスプライト差し替え
- rpg193 inventory Bank11移動

## 確認ポイント
ビルド後、以下が出ないことを確認してください。

- `Write from one bank spans into the next`
- `Multiple write`
- `Possible overflow`
- romusage の `Overflow by ... bytes`

特に romusage で `_CODE_7` が 0x77fff を超えていないことを確認してください。

## 注意
この環境ではGBDKの実ビルドは未実行です。
