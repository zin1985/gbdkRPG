# rpg131 quest header compile fix

## 修正内容

`main.c` から `quest_start(QUEST_LOST_KEY)` / `quest_advance(QUEST_LOST_KEY)` を呼び出しているにもかかわらず、`quest.h` が include されていなかったため、SDCC/GBDK 上で implicit declaration となり、`too many parameters` エラーになっていた。

## 変更ファイル

- `main.c`

## 変更点

```c
#include "quest.h"
```

を既存 include 群へ追加。

## Bank 0 影響

ヘッダ include のみで、新規処理本体・巨大データ・文字列追加はなし。
Bank 0 を増やす方向の修正ではない。

## 期待する解消エラー

- `function 'quest_start' implicit declaration`
- `quest_start(...) too many parameters`
- `function 'quest_advance' implicit declaration`
- `quest_advance(...) too many parameters`
