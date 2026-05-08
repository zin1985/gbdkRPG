# CHECK rpg136_scheduled_bank_architecture_pack

## 目的

スケジュール検討で作成してきたバンク構想を現行プロジェクトへ集約する。

## 変更内容

- ROM出力名を `rpg136_scheduled_bank_architecture_pack.gb` に変更。
- ROMバンク数を `-Wl-yo16` に拡張。
- `bank_ids.h` を追加。
- `field_feature_runtime.c` を bank 7 から bank 5 へ移動。
- `docs/design/00_handover_bank_architecture_rpg136.md` を追加。
- `docs/design/01_future_feature_to_bank_map.md` を追加。
- `docs/future/RPG_FEATURE_FULL_LIST_BANK_POLICY.md` を追加。
- `tools/bank_architecture_check.py` を追加。

## 重要

未来機能を大量に実処理として一気に足した版ではない。
Bank 0とWRAMを守るため、まずバンク構想・配置・検査の土台を実装した版。

## 確認コマンド

```sh
chmod +x build.sh
./build.sh
python3 tools/bank_architecture_check.py
```
