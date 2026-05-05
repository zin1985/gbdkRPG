# rpg075_fix


<details open>
<summary>rpg075_fix の要点</summary>

- rpg074_ui の `main.c:1689: error 12: called object is not a function` を修正。
- 原因は `display_off` という引数名が GBDK の `DISPLAY_OFF` マクロ展開内の `display_off()` と衝突したこと。
- 引数名を `turn_display_off` に変更して衝突を回避。
- rpg074で入れた2人分パーティ枠、敵名長18、初回コマンド前再描画は維持。

</details>

> **長期目標:** GBDK/Game Boyの制約内で、SaGa風戦闘、ランダム成長、技ひらめき、魔法、属性、状態異常、町・村・ダンジョン、チャンク制フィールド、最終的に5000x5000相当の世界設計へ段階拡張する。巨大配列常駐ではなく、チャンク制・セクター制・ROMバンク分割・必要範囲だけのロード/描画・メタタイル圧縮を前提にする。

## rpg075今回の要点

<details open>
<summary>今回の変更</summary>

- 戦闘左上UIを `ゆうしゃ` + `そうりょ` の2枠表示へ拡張。
- `そうりょ` は表示だけの仮枠で、戦闘参加・仲間処理は未実装。
- 敵名バッファを `18u` に拡張。
- 敵名コピーをUTF-8途中切れしにくい処理に変更。
- 初回コマンド前の戦闘画面再構築を `battle_reset_screen_for_command()` に集約。
- 新規BGタイル、MAP_GFX_TILE_COUNT、新規actor、新規object kind、VRAMロード経路は変更なし。

</details>

# rpg017

## 方針

rpg015の動作確認済み安全構成を基準に、スプライト追加前の調査版として作成。

- スプライト追加なし
- 新規BGタイル追加なし
- MAP_GFX_TILE_COUNT変更なし
- 新規object kind追加なし
- 新規actor/NPC追加なし
- VRAMロード経路変更なし
- main.cのステータス画面内デバッグ表示（位置/カメラ）を削除
- build.bat / build.sh / Makefile に `-Wl-m -Wl-j` を追加し、map/symbol出力を有効化

## Linuxビルド

```sh
chmod +x build.sh
./build.sh
```

または:

```sh
make clean
make
```

GBDKの `lcc` は `/opt/gbdk/bin/lcc` を想定。環境が違う場合は `PATH` または `GBDK_PATH` を調整。

## 目的

rpg013/rpg014/rpg016でスプライト追加により白画面化したため、先にmain.cを軽くし、`.map` を出してHOME bank/ROM配置の余裕を確認する。

## 次の段階

rpg017が白画面化しないことを確認してから、rpg018で `init_map_sprites()` に以下2行だけを追加する。

```c
set_sprite_tile(12u, NPC0_TILE_BASE);
move_sprite(12u, 80u, 80u);
```


## rpg022: BG森マーカー（通行判定なし）

rpg022では、スプライトではなくBGとして森マーカーを1箇所だけ描画します。

安全方針:

- 新規BGタイルは追加しない
- MAP_GFX_TILE_COUNTは13uのまま
- 新規object kindは追加しない
- collision16_mapは変更しない
- スプライト/OAMは追加しない
- draw_object_map()の最後でbg[]へ見た目だけ上書きする

現時点では既存の町マーカーBGタイルを森の仮表示として流用します。
添付画像風の専用森タイル追加は、rpg022が安定確認できてから検討します。


## rpg024: 町マーカー維持 + 森BG追加

rpg023では町マーカーのタイル枠を森に差し替えたため、フィールドの町表示と森表示が同じ枠を取り合っていました。

rpg024では、町マーカーを 9〜12 に残し、森専用BGタイルを 13〜16 に追加しています。

安全方針:

- スプライト/OAMは使わない
- 新規object kindは追加しない
- collision16_mapは変更しない
- 森はdraw_object_map()のBG上書きのみ
- 通行判定は床のまま
- MAP_GFX_TILE_COUNTは17uに増加

この変更は「町と森を区別するために必要な最小のBGタイル追加」です。


## rpg030: グラフィック安全改修版

rpg030では機能追加ではなく、今後の拡張に備えてグラフィック周りの安全ルールを固定しました。

今後増える予定のもの:

- フィールドサイズ
- フィールドBGタイル
- NPCスプライト
- 戦闘敵スプライト
- 町オブジェクト、壺、宝箱など

方針:

- 静的なものはまずBG/metatile化
- 動くものだけOAM/OBJスプライト化
- BGタイル追加、object kind追加、collision変更、OAM追加を同時に行わない
- `main.c` の `GBDK RPG GRAPHICS SAFETY CHECKPOINT` を読んでから修正する


## rpg046: 非表示actor枠テスト

rpg045_fixでも白画面になったため、新しい関数追加をやめ、rpg030を基準にデータだけでOAM 12〜15を確認します。

追加内容:

- `TEST_ACTOR_SPRITE_BASE = 12u`
- `actors[]` に `active=0` の非表示actorを1件追加
- 既存の `draw_all_actors()` が inactive actor を非表示化する

行っていないこと:

- 新規スプライト画像追加
- `set_sprite_data()` 変更
- `set_sprite_tile()` 追加
- 直接 `move_sprite()` 追加
- BGタイル追加
- `MAP_GFX_TILE_COUNT`変更

見た目がrpg030と同じで、白画面にならないことが正解です。


## rpg047: 既存NPCグラフィック流用の表示テスト

rpg047では、rpg046で追加した非表示actor枠を、表示だけONにします。

- `active`: `0u` → `1u`
- `solid`: `0u` のまま
- `talkable`: `0u` のまま
- `sprite_base`: `TEST_ACTOR_SPRITE_BASE = 12u`
- `tile_base`: `NPC0_TILE_BASE`

つまり、新しい画像データは追加せず、既存NPCグラフィックを流用した飾りスプライトを表示します。

この版では、会話・衝突・新規画像追加は行いません。


## rpg048b: 会話可能フラグだけの分離テスト

rpg048が白画面になったため、変更を分解しました。

この版では、rpg047で表示できた追加actorに対して、`talkable` を `0u` から `1u` に変えるだけに近い構成にしています。  
`try_interact()` のロジック変更は入れていません。

目的は、白画面の原因が `talkable=1` なのか、それとも `try_interact()` のコード追加なのかを切り分けることです。


## rpg049: 安全イベント方針の固定

rpg049では、rpg048bの正常動作を基準に、今後の安全な機能追加ルールを固定します。

rpg048では白画面が発生しましたが、rpg048bでは `talkable=1` のデータ変更のみで正常動作しました。  
そのため、現時点では `talkable=1` 自体ではなく、`try_interact()` の分岐追加、新規文字列追加、またはコード/データ配置変化が白画面原因候補です。

当面の安全方針:

- actor追加と会話分岐追加を同時に行わない
- 新規文字列追加と分岐追加を同時に行わない
- `MAP_GFX_TILE_COUNT` を変更しない
- `set_bkg_data()` / `set_sprite_data()` を変更しない
- WINDOW/printf/gotoxy/画面デバッグを追加しない
- 新規BGタイル追加は保留する

この版では、ゲーム挙動の追加は行っていません。


## rpg051: build/romusage確認基盤

rpg051では、ゲーム挙動を変えず、ビルド時にROM配置・バンク・overflow系の情報を見やすくするための構成整理を行っています。

追加したこと:

- `build.log` を出力
- ビルドログ内の warning / overflow / ASlink / bank / relocation / segment / spans を確認
- `romusage` が使える場合はROM使用量を出力
- `.map` / `.noi` / `.sym` の生成を継続

変更していないこと:

- `main.c` のゲーム挙動
- actor数
- 文字列
- `try_interact()`
- スプライト
- BGタイル
- `MAP_GFX_TILE_COUNT`
- collision/object map

rpg050が短文追加だけで白画面になったため、今後は機能追加前に `build.log` と `romusage` で固定バンク・バンク境界・リテラル領域の状態を確認します。


## rpg055: メッセージのバンク分離

rpg055では、rpg050で発生したバンク境界またぎ警告を踏まえ、会話文字列を `main.c` から分離しました。

rpg050で出ていた危険警告:

```text
Warning: Write from one bank spans into the next. 0x7fff -> 0x8001 (bank 1 -> 2)
```

対応内容:

- `messages.h` を追加
- `messages_bank.c` を追加し、会話文字列をROM bank 2へ配置
- `messages_runtime.c` を追加し、banked文字列をWRAMへコピーしてから表示
- `Actor` は `const char *message` ではなく `UINT8 message_id` を持つ
- `main.c` へ新規文字列を追加しない構成へ変更
- ビルドスクリプトで危険なバンク警告を失敗扱いにする

注意:

コンパイル前に正確なバンク溢れを完全判定することはできません。  
最終的な配置はコンパイル・リンク後に決まるためです。  
その代わり、`tools/prebank_check.py` による事前ヒューリスティック確認と、ビルド後の `build.log` / `romusage` 確認を組み合わせています。


## rpg056: グラフィックデータのバンク分離

rpg055でも bank 1 → bank 2 の溢れが発生したため、さらに大きいデータである `sprites.c` のグラフィック配列を ROM bank 3 へ移動しました。

追加:

- `banked_graphics.h`
- `banked_graphics.c`

変更:

- `sprites.c` に `#pragma bank 3` と `BANKREF(sprite_data_bank)` を追加
- `main.c` の `set_bkg_data()` / `set_sprite_data()` 直接呼び出しをやめる
- `set_banked_bkg_data()` / `set_banked_sprite_data()` 経由でVRAMへ転送

狙い:

- bank 1 の容量を空ける
- bank 2 のメッセージデータとの衝突を避ける
- 今後のメッセージ・スプライト・BG追加の土台を作る


## rpg057: build warning check修正

rpg056では、実際のコンパイル出力にbank overflowが出ていないにもかかわらず、`build.log` 自身をgrepしていたため false positive で失敗していました。

rpg057では以下のように分離しています。

- `precheck_output.log`
- `compile_output.log`
- `build.log`

危険警告の判定は `compile_output.log` のみを対象にします。

また、`prebank_check.py` はCコメントを除去してからチェックするようにしたため、コメント中の `set_bkg_data()` / `set_sprite_data()` で誤警告しにくくなっています。


## rpg058: マップデータのバンク分離

rpg057ではコンパイラ出力上のbank span警告は消えましたが、`romusage` で Bank 0 の overflow 可能性が出ていました。

rpg058では、`main.c` に残っていた 16x16 の collision/object map 配列を `map_data_bank.c` へ移動しました。

追加:

- `map_data.h`
- `map_data_bank.c`
- `map_data_runtime.c`

目的:

- `main.c` から大きい静的マップ配列を逃がす
- Bank 0 / non-banked area の圧迫を軽減する
- romusage警告をビルド失敗扱いにして、白画面ROMを通さない


## rpg059: console/stdio依存の削除

rpg058でも `romusage` で Bank 0 overflow 警告が残ったため、Bank 0を圧迫しやすい console/stdio 系の使用をやめました。

削除:

- `#include <gbdk/console.h>`
- `#include <stdio.h>`
- `#include <string.h>`
- console clear/cursor/output helper calls
- formatted output helper calls

代替:

- `jp_put_bkg_text()`
- `screen_clear()`
- `put_cursor()`
- `put_u8()`
- `put_hp_pair()`

また、`map_data.h` の include guard と `MAP_DATA_H` 高さ定数が衝突していたため、`MAP_DATA_HEIGHT` に改名しています。


## rpg060: helper定義漏れ修正

rpg059では、console/stdio削除のために `put_u8()` などへ置換しましたが、関数本体の挿入が漏れていたため、リンク時に undefined global が発生しました。

rpg060では以下の関数本体を `main.c` に追加しています。

- `put_ascii()`
- `put_cursor()`
- `u8_to_dec()`
- `put_u8()`
- `put_hp_pair()`
- `screen_clear()`

ゲーム内容・BG・OAM・actor・object kindは変更していません。


## rpg061: jpfont/misakiのバンク分離

rpg060でも Bank 0 の romusage 警告が残ったため、日本語フォント描画系を ROM bank 5 へ移動しました。

対象:

- `jpfont.c`
- `misakiUTF16.c`
- `misakiUTF16FontData.h` 由来のフォントデータ

変更:

- `jpfont.c` に `#pragma bank 5`
- `misakiUTF16.c` に `#pragma bank 5`
- `jpfont.h` の公開APIを `BANKED` に変更
- `jpfont.c` の公開関数定義も `BANKED` に変更

目的:

- Bank 0 / non-banked area の圧迫を減らす
- 文字描画・会話・メニュー表示の処理をbanked側へ逃がす


## rpg062: misaki BANKREF include修正

rpg061では `misakiUTF16.c` に `BANKREF(misaki_bank)` を追加しましたが、`BANKREF` マクロを定義する `gb/gb.h` をincludeしていなかったため、コンパイルエラーになりました。

rpg062では `misakiUTF16.c` の先頭に以下を追加しています。

```c
#include <gb/gb.h>
```

jpfont/misakiを bank 5 へ逃がす設計自体は維持しています。


## rpg063: misakiをbank 6へ分離

rpg062では `jpfont.c` と `misakiUTF16.c` を同じ bank 5 に置いたため、bank 5 から bank 6 へ少しだけ溢れました。

```text
Warning: Write from one bank spans into the next. 0x17fe4 -> 0x18003 (bank 5 -> 6)
```

rpg063では以下のように分けています。

- `jpfont.c`: ROM bank 5
- `misakiUTF16.c`: ROM bank 6

`jpfont.c` から `misakiUTF16.c` の関数を呼ぶため、`misakiUTF16.h` の公開関数は `BANKED` 宣言に変更しています。


## rpg064: jpfontは非bankedへ戻し、misakiだけbank 6

rpg063では起動はできましたが、NPC会話や町移動時のメッセージ表示でフリーズしました。

原因候補は、`jpfont.c` を BANKED 化した状態で、さらに `misakiUTF16.c` の複数の BANKED 関数を呼んでいたことです。

rpg064では以下に戻しています。

- `jpfont.c` は非banked
- `jpfont.h` の公開APIも非BANKED
- `misakiUTF16.c` は bank 6 のまま
- `jpfont.c` から misaki 側へは `getFontData()` だけを BANKED 呼び出し

これにより、メッセージ表示中の複雑なネストBANKED呼び出しを避けます。


## rpg065: フォントデータのみbank 6へ分離

rpg064でもメッセージ表示時にフリーズしたため、BANKED関数呼び出しをメッセージ描画経路から外しました。

- `jpfont.c` は非banked
- `misakiUTF16.c` のコードも非banked
- 巨大な `misakiUTF16FontData.h` のテーブルだけを `font_data_bank.c` として bank 6 へ配置
- `font_data_runtime.c` の NONBANKED wrapper 経由で必要な数バイトだけ読む


## rpg066: getFontTableAddressのfdata参照修正

rpg065では、巨大フォントデータ `fdata` / `ftable` を `font_data_bank.c` へ移動しましたが、`misakiUTF16.c` に残っていた `getFontTableAddress()` が `fdata` を直接返そうとしてビルドエラーになっていました。

rpg066では、互換用に関数だけ残し、戻り値をNULLに変更しています。

```c
const uint8_t*getFontTableAddress(void) {
    return (const uint8_t*)0;
}
```

実際のフォント取得は `font_data_runtime.c` のNONBANKED wrapper経由で行います。


## rpg067: 町・フィールド別actor表示方針

rpg067では、rpg066の安定状態を基準に、町とフィールドで表示するactorを分けます。

- `AREA_FIELD` では enemy actor を表示し、NPC actor を非表示にします。
- `AREA_TOWN` では NPC actor を表示し、enemy actor を非表示にします。
- 非表示actorは移動判定・会話判定・戦闘判定の対象外にします。
- OAMスロットは増やしません。
- 新規BGタイル、スプライト、object kind、actorは追加しません。

この方針により、今後の町NPC追加・敵追加・フィールド拡張を、同じOAM枠で段階的に行えるようにします。


## rpg068: フィールドランダムエンカウント

rpg068では、フィールド上の戦闘をランダムエンカウント方式に変更しました。

- フィールドで1歩移動完了後にランダム判定を行います。
- 町ではランダムエンカウントしません。
- 町入口などのイベントタイル上ではランダムエンカウントしません。
- 既存のenemy actorは戦闘テンプレートとして使います。
- enemy actorはフィールド上では非表示にします。
- 戦闘開始時にパレット点滅の簡易エフェクトを入れています。
- 新規BGタイル、スプライト、actor、object kindは追加していません。


## rpg069: 文字化け・戦闘画面表示荒れ修正

rpg068で戦闘画面や会話に文字化けが出たため、`jpfont.c` のキャッシュキーを一時的なバイト列ベースから、元の正規化UTF-16ベースへ戻しました。

また、戦闘画面では以下を行います。

- 戦闘画面描画前に `jp_init()` でフォントキャッシュをリセット
- 戦闘開始メッセージ後に戦闘画面を再描画
- ASCII罫線を削除して、不要なグリフキャッシュ消費を減らす
- マップ復帰時にもLCD OFF中に `jp_init()` してウィンドウ/キャッシュを整理

新規BGタイル・スプライト・actor・object kindは追加していません。


## Documentation folders

rpg070 moved project notes and check documents into `docs/` to keep the root
folder focused on source files and build entry points.

Start here:

- `docs/README_DOCS.md`
- `docs/checks/`
- `docs/bank/`
- `docs/graphics_safety/`
- `docs/features/`


## rpg071: Battle encounter table

rpg071 cleans the battle screen fully before command input and adds a banked enemy encounter table.

- Full 32x32 battle BG clear
- Boxed command UI
- Lower-left enemy name box
- Existing enemy OBJ sprites shown during battle
- Up to two enemies per encounter
- Battle data stored in bank 7


## rpg072: Battle helper definition fix

rpg072 fixes the rpg071 linker error by adding the missing battle helper function bodies to `main.c`.

No new BG tiles, map actors, object kinds, or sprite sheets were added.
