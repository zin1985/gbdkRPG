# GBDK + GitHub Actions 自動ビルド雛形

## 入っているもの
- `.github/workflows/build-gb.yml`
- `Makefile`
- `src/main.c`
- `.gitignore`

## 使い方
1. この一式を GitHub リポジトリのルートに置く
2. `main` か `master` に push する
3. GitHub の `Actions` タブで `Build Game Boy ROM` を開く
4. 成功すると `Artifacts` に `.gb` が出る

## 手動実行
`workflow_dispatch` を入れてあるので、Actions 画面の `Run workflow` から手動実行できます。

## GBDK バージョン
ワークフローは `GBDK_TAG: gbdk-4.5.0` を使う設定です。
別バージョンにしたい場合は `.github/workflows/build-gb.yml` の `GBDK_TAG` を変更してください。

## ビルド対象
Makefile は Game Boy 向けに `-msm83:gb` でビルドします。

## よく変える場所
- ROM名: `Makefile` の `PROJECT`
- ソース追加: `src/` に `.c` を増やす
- ビルドオプション: `Makefile` の `CFLAGS`
