# rpg227_magic_skill_duplicate_fix

## 目的
rpg226で発生した以下のビルドエラーを修正する。

- `main.c:1259: error 65: function 'skill_is_magic' already has body`
- `main.c:1263: error 65: function 'skill_is_heal_magic' already has body`

## 原因
`main.c` の前方宣言エリアに、`skill_is_magic()` と `skill_is_heal_magic()` の関数本体を誤って追加していた。
その後、実装本体側にも同じ関数が存在したため、SDCC/GBDK上で「already has body」になっていた。

## 修正内容
前方宣言エリアの2関数を、本体ではなくプロトタイプ宣言へ修正した。

```c
static UINT8 skill_is_magic(UINT8 skill_id);
static UINT8 skill_is_heal_magic(UINT8 skill_id);
```

実装本体は `calc_attack_damage()` の後ろに1箇所だけ残している。

## 追加確認
簡易静的チェックで、`main.c` 内の重複 static 関数本体が無いことを確認済み。

## 注意
この環境にはGBDK `lcc` が無いため実ビルドは未確認。
ローカルでは次を確認すること。

```sh
./build.sh
```

危険警告が出た場合は失敗扱い。

- Possible overflow from Bank 0 into Bank 1
- Multiple write
- Write from one bank spans into the next
- Undefined Global
- Bank overflow
