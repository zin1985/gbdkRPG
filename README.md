# rpg082_dirty: 戦闘 dirty update 化

`rpg082_dirty` は、戦闘中の白画面化・カーソルちらつき・再描画遅延を避けるため、戦闘画面の全再描画を廃止する方向へ寄せた版です。

## 目的

- 戦闘突入時だけ `battle_enter_render_once()` で BG / OAM / 固定枠 / 敵 / 味方欄を一度だけ構築する。
- 以後は dirty flag で、HP/MP数値、OBJカーソル、下部メッセージ欄、敵OAM表示だけを差分更新する。
- コマンド選択、カーソル移動、敵攻撃、味方攻撃、メッセージ送りで戦闘画面全体を作り直さない。

## 主な変更

- `battle_dirty_flags` / `battle_update_dirty()` を追加。
- `battle_enter_render_once()` を追加し、戦闘突入時の一回描画に集約。
- `battle_reset_screen_for_command()` / `battle_prepare_intro_screen()` / `battle_redraw_after_dialogue()` の戦闘中再描画経路を廃止。
- コマンドカーソルをBG文字 `>` から OBJ 24 の8x8スプライトへ変更。
- `draw_battle_menu()` は静的BG文字だけを描画する方式へ変更。
- 下部3行をメッセージ専用の差分更新領域として確保。
- 敵名欄を rows 11-14 に縮め、rows 15-17 をメッセージ領域として分離。
- 敵攻撃・味方攻撃・回復・逃走・勝利/敗北メッセージを、Windowではなく戦闘BG下部メッセージ欄に表示。
- 敵ダメージ時は対象敵スプライトだけを短く非表示にして点滅風に見せる。
- OBJカーソル用の `battle_cursor_tiles` を `sprites.c` / `sprites.h` に追加。

## 維持した制約

- 新規BGタイル追加なし。
- `MAP_GFX_TILE_COUNT` 変更なし。
- 新規object kind追加なし。
- 新規actor/NPC追加なし。
- `jpfont.c` / `misakiUTF16.c` のBANKED化なし。
- `main.c` から直接 `set_bkg_data()` / `set_sprite_data()` しない。

## ビルド注意

`build.sh` / `build.bat` / `Makefile` の出力名は `rpg082_dirty.gb` に変更しています。
この作業環境では GBDK `lcc` が存在しないため、実コンパイルは未確認です。`prebank_check.py` はOKです。

---


## rpg079_party: battle redraw hotfix

- 敵攻撃前に白画面が出る原因になっていた通常ターン中の `DISPLAY_OFF` 再描画を停止。
- コマンドカーソル移動時はメニュー全体ではなくカーソル位置だけ更新。
- `かいふく` / `にげる` の下に出ていた使用回数表示行を削除し、ゴミ文字に見える表示を排除。
- 敵ターン終了後、次のコマンド入力前に Window を画面外へ退避し、BGスクロールを `0,0` に戻してからUIを再構築。

# rpg077_battle
> 最新版: `rpg077_battle` では、戦闘UIの横幅圧迫を減らすため味方パラメータ表示から `/最大値` を外し、敵を最大3体表示へ拡張し、戦闘用の新しい敵OBJスプライト3種を追加しました。戦闘コマンドカーソルは上下左右で2x2移動できます。

> **長期目標:** GBDK/Game Boyの制約内で、SaGa風戦闘、ランダム成長、技ひらめき、魔法、属性、状態異常、町・村・ダンジョン、チャンク制フィールド、最終的に5000x5000相当の世界設計へ段階拡張する。巨大配列常駐ではなく、チャンク制・セクター制・ROMバンク分割・必要範囲だけのロード/描画・メタタイル圧縮を前提にする。

<details open>
<summary>rpg077_battle の要点</summary>

- 味方パラメータ表示を `HP 38` / `MP 38` 形式へ変更し、`38/38` 形式を廃止。
- `そうりょ` は引き続き表示だけの仮枠。戦闘参加・actor追加はなし。
- 敵同時出現数を最大3体へ拡張。
- 敵OBJスプライトをスライム風・こうもり風・けもの風の3種類へ差し替え。
- 戦闘中のみ敵OBJタイル先頭12枚をロードし、フィールド復帰時は `init_map_sprites()` で通常状態へ戻す。
- 戦闘コマンドカーソルを上下左右移動可能に変更。
- 新規BGタイル、MAP_GFX_TILE_COUNT、新規actor/NPC、新規object kindは変更なし。

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


## rpg080_icons
- 戦闘パーティ表示の顔アイコンをユーザー添付画像へ差し替えました。


## rpg082_dirty

戦闘UIの残像・ズレ対策版です。

- 魔物登場メッセージ時点で味方3人分のパラメータ欄を表示します。
- 味方パラメータ欄のキャラごとの縦区切り線を削除しました。
- 味方3人の16x16表示アイコンを、M数値右側に一段上げて配置しました。
- フィールドプレイヤーのOAM残りを避けるため、戦闘遷移前と戦闘画面再構築時に全OAMを一度退避します。
- 敵は毎回3体表示されるように、遭遇テーブルを3体構成へ寄せました。
- 敵攻撃前後の長い白画面対策として、戦闘中のコマンド画面再構築では DISPLAY_OFF を使わない方針に変更しました。

### rpg081の確認観点

- 魔物登場時点で、上部パラメータ欄と敵スプライトが同時に出ているか。
- 最初のコマンド画面で、BG座標が左上からずれないか。
- フィールドプレイヤーが中央などに残っていないか。
- 敵攻撃前後で白画面が長く挟まらないか。
- カーソル移動時に文字全体が再描画ちらつきしないか。

## rpg083_music

フィールド・町・戦闘のBGMを、軽量なGB音源レジスタ直叩き方式で追加しました。

目的は、SaGa風の「場面ごとに空気が変わるRPG感」を入れることです。既存曲のコピーではなく、以下の方向性のオリジナル短ループです。

- フィールド: 冒険感のある短い行進風ループ
- 町: ゆっくりした落ち着いたループ
- 戦闘: テンポが速い緊張感のあるループ

追加ファイル:

- `audio.h`
- `audio.c`

変更点:

- `main.c` で `audio_init()` / `audio_play_music()` / `audio_update()` を呼び出すようにしました。
- 起動後はフィールドBGMを再生します。
- 町に入ると町BGMへ切り替えます。
- 町から出るとフィールドBGMへ戻します。
- 戦闘突入時に戦闘BGMへ切り替えます。
- 戦闘終了後は現在エリアに応じてフィールド/町BGMへ戻します。
- 会話・メニュー・戦闘メッセージ待ち中もBGMが止まりにくいよう、主要な `waitpad()` / `wait_vbl_done()` 待ちを `audio_waitpad()` / `audio_wait_vbl()` 経由へ変更しました。

安全方針:

- 新規BGタイルは追加していません。
- `MAP_GFX_TILE_COUNT` は変更していません。
- 新規actor/NPC/object kind は追加していません。
- `jpfont.c` / `misakiUTF16.c` のBANKED化はしていません。
- hUGEdriverなどの外部音楽ドライバはまだ追加していません。

注意:

- 今回は白画面リスクを避けるため、まずは小さい手作りBGMエンジンです。
- 本格的な作曲データ化を行う場合は、次段階で hUGEtracker / hUGEdriver 方式を検討します。
- 実機/VBAでは、音量が大きい場合があります。その場合は `audio.c` の `NR50_REG = 0x66u;` を `0x44u` などに下げて調整してください。

## rpg084_window_right10

### 目的

戦闘中のBGウィンドウがフィールド側のBGスクロール状態の影響でずれる問題に対して、戦闘UIのBG描画原点を右へ10マス補正する。

### 実装方針

- 戦闘BGウィンドウ描画用に `BATTLE_BG_SHIFT_X = 10` を追加。
- 戦闘中はBG描画座標を `BATTLE_BG_X(x)` 経由で10マス右へ寄せる。
- 同時に `move_bkg(BATTLE_BG_SCROLL_X, 0)` を使い、戦闘BGの参照位置を10マス分合わせる。
- OBJの敵・味方アイコン・カーソル位置は既存の画面座標を維持する。
- 変更対象は戦闘UIのBG描画だけに限定し、音楽・フォント・マップ・スプライトデータ構造は変更しない。

### 注意

この版は、戦闘UIのずれ補正を最小範囲で行うための版です。GBDK実機ビルド環境がこの作業環境にないため、`lcc` による最終ビルドは未確認です。


## rpg085_window_left3

目的:
- `rpg084_window_right10` で右へ10マス補正した戦闘BGウィンドウを、ユーザー確認結果に合わせて3マス左へ戻す。

変更内容:
- `BATTLE_BG_SHIFT_X` を `10u` から `7u` に変更。
- `BATTLE_BG_SCROLL_X` は `BATTLE_BG_SHIFT_X * 8` のため、80px相当から56px相当に自動で変更。
- 戦闘UIのBG描画補正は `BATTLE_BG_X(x)` 経由のまま維持。

修正していない箇所:
- BGM処理。
- dirty update 構造。
- OBJの敵・味方アイコン・カーソル座標。
- `MAP_GFX_TILE_COUNT`。
- `jpfont.c` / `misakiUTF16.c`。

確認観点:
- 戦闘中ウィンドウが前回より3マス左に戻っていること。
- 初回コマンド時にズレが再発していないこと。
- OBJカーソル、敵、味方アイコンが大きくズレて見えないこと。


## rpg086_window_reset

目的:
- rpg084/rpg085で追加した戦闘BGの右寄せ補正をいったん解除し、戦闘ウィンドウ基準位置をリセットする。
- `BATTLE_BG_SHIFT_X` を `0u` に戻し、`move_bkg()` の戦闘時スクロールも `0px` 相当に戻す。

変更:
- `main.c`: `BATTLE_BG_SHIFT_X 7u` → `0u`
- BG描画マクロ `BATTLE_BG_X(x)` は残すが、現時点では加算なしとして動作する。

修正していない箇所:
- dirty update構造
- BGM処理
- OBJの敵・味方アイコン・カーソル座標
- MAP_GFX_TILE_COUNT
- jpfont.c / misakiUTF16.c

確認ポイント:
- 戦闘突入時のパーティ欄、敵名欄、コマンド欄、メッセージ欄が画面内の自然な位置に戻ること。
- BG枠と文字がずれずに重なること。
- 敵・味方アイコン・カーソルOBJが大きく外れないこと。

## rpg087_growth_rank

### 目的

`rpg086_window_reset` を基準に、スレッドで検討した成長システム拡張のうち、すぐ実装可能な土台を追加した版です。

### 追加内容

- HP/MPの上限を `999` に変更。
- その他能力値の上限を `255` に変更。
- 主人公に `MP` ステータスを追加。
- 戦闘画面の主人公 `M` 表示を固定値ではなく `player_battle.mp` 表示へ変更。
- 成長候補を `HP / MP / ちから / まもり / とくぎ / かいふく / すばやさ` に拡張。
- `GrowthResult` 構造体を追加し、成長前後の値を記録する土台を追加。
- `battle_enemy_rank` を追加し、戦闘開始時点の敵の強さを保存。
- rank別成長率テーブルを追加。
- 上限到達時に別能力へ流れ込む挙動を廃止。

### 制約

- 複数キャラ成長は未実装。
- 技ひらめき、武器熟練度、GrowthResult配列化は未実装。
- MPはステータスとして追加したが、現時点では `とくぎ` / `かいふく` の使用回数制からMP消費制へは変更していない。
- 戦闘UIの描画方式、BGM、MAP_GFX_TILE_COUNT、jpfont.c、misakiUTF16.c は変更していない。

## rpg088_mp_skill

目的: rpg087_growth_rank の成長システム拡張を前提に、スレッドで検討したMP消費制・SkillDefテーブル方式・軽量な効果計算式を取り込む。

### 主な変更
- `とくぎ` / `かいふく` を使用回数制からMP消費制へ移行した。
- とくぎの消費MPは2、かいふくの消費MPは4とした。
- MP不足時は「Mがたりない」と表示し、敵ターンへ進めずプレイヤー選択へ戻す。
- HP満タン時のかいふくは「Hまんたん」と表示し、MPを消費しない。
- `SkillId` / `SkillKind` / `TargetType` / `SkillDef` を追加し、将来の複数技テーブル化に備えた。
- 通常攻撃、とくぎ、かいふくの効果計算を分離した。
- ダメージ計算は `INT16` を経由し、最低1ダメージを保証する。
- 成長メッセージ文字列を `messages_bank.c` へ移動し、Bank 0圧迫を軽減した。

### 効果式
```c
通常攻撃 = attack * 2 - defense
とくぎ   = skill * 3 + attack + power - defense / 2
かいふく = heal * 2 + power
```

### Bank 0注意
前回の `rpg087_growth_rank` ではビルド自体は完了したものの、`romusage` により `_HOME` overflow が検出された。今回の変更では、追加コードを最小限にしつつ、成長メッセージ文字列をbank 2へ逃がしてBank 0増加を抑えている。

## rpg090_battle_window_fix

目的: rpg088_mp_skill を基準に、見た目の戦闘メニューを変えず、内部だけを複数技対応へ寄せる。

### 主な変更
- `SkillDef` を拡張し、id / kind / target / mp_cost / power / accuracy / flags を持つ構造へ変更。
- 将来の複数技・状態異常・敵AI利用に備え、`SKILL_FIRE` / `SKILL_GUARD_BREAK` などのIDを予約。
- `PlayerSkillSet` を追加し、固定4スロット方式の土台を導入。
- 既存の「とくぎ」は skill slot 0、「かいふく」は skill slot 1 を参照するように変更。
- `player_use_skill(skill_id)` に、とくぎ/かいふくの実行処理を集約。
- rpg088に残っていた `player_run()` の重複 else 由来の構文不整合を修正。

### まだ追加していないもの
- 技一覧UI
- 対象選択UI
- 技習得処理
- 状態異常
- セーブデータへのskill slot保存
- SkillDefのbanked data化

### Bank 0注意
main.cのコード量は増えているため、ユーザー環境で `./build.sh` を実行し、romusageで `_HOME` overflow が出ないか必ず確認する。

## rpg090_battle_window_fix

Battle window/UI position reset patch.

- Forces battle BG origin to `(0,0)` with `battle_reset_bg_origin()`.
- Removes reliance on old right-shift SCX compensation during battle.
- Keeps the rpg089 skill-slot internal structure unchanged.
- Does not change MAP_GFX_TILE_COUNT, jpfont.c, misakiUTF16.c, OAM allocation, or BG tile data.


## rpg100_party_roster_fix

将来RPG機能の土台として、以下の小型banked moduleを追加。

- `game_flags.c/h`
  - 512bit / 64byte のイベントフラグ管理
  - `game_flag_set()`, `game_flag_clear()`, `game_flag_get()`
  - セーブ用 `copy_to/copy_from`

- `quest.c/h`
  - 16件分のクエスト状態管理
  - `QST_NONE / QST_START / QST_PROG / QST_READY / QST_DONE`
  - `QUEST_HERB`, `QUEST_SLIME`, `QUEST_LOST_KEY` の初期ID

- `inventory.c/h`
  - 64種類分の簡易所持数管理
  - 最大99個スタック
  - `ITEM_HERB`, `ITEM_POTION`, `ITEM_CASTLE_KEY` の初期ID

これらはまだ既存ゲーム進行へ強く接続していない。
既存戦闘UI安定版を壊さないため、まずは将来実装用APIとして追加している。


## rpg100_party_roster_fix

`rpg100_party_roster_fix` のビルドエラー修正版。

修正内容:
- `show_one_battle_enemy_sprite()` / `hide_one_battle_body()` のプロトタイプを追加し、初回コマンドUI補正関数内の暗黙宣言を解消。
- `battle_text.c` に混入していた実NUL文字/実改行文字リテラルを除去し、`'\0'`, `'\n'`, `'\r'` の通常表記へ修正。
- rpg097で追加した `game_flags.c`, `quest.c`, `inventory.c` は維持。


## rpg100_party_roster_fix

戦闘パーティ表示アイコン `battle_party_display_tiles` を、ユーザー提供の 16x16 画像 3枚から差し替え。

変換ルール:
- 外周からつながっている白背景は透明 (OBJ color 0)
- 純白は明色
- 純黒は黒
- 黒でも白でもない色はすべて中間色へ寄せる

対象:
- まほう
- ゆうしゃ
- そうりょ


## rpg100_party_roster_fix

- JP font cacheを48→96へ拡張し、長い戦闘メッセージ後に上部の仲間名が別文字へ化ける症状を抑制。
- パーティステータス欄を固定値ではなく `party_roster[]` から描画するよう変更。
- アクティブ仲間3名: ゆうしゃ / そうりょ / まほう。
- 予備仲間3名: せんし / かりうど / ぶとうか。
- `party_swap_active_with_reserve()` を追加し、将来の入れ替えUIから呼べる下準備を実装。
- 敵攻撃がアクティブ仲間の生存者からランダムに対象選択するよう変更。
- ゆうしゃとまほうの戦闘アイコン表示を交換。


## rpg101_field_player_sheet_replace

ユーザー提供の 12x4 スプライトシートを分割し、プレイヤー系フィールドスプライトへ反映。

### 分割ルール
- 横12分割 / 縦4分割
- 左上から連番
- 1-12   = ゆうしゃ
- 13-24  = まほう
- 25-36  = そうりょ
- 37-48  = 4人目の仲間

### 採用フレーム
各キャラの 3x4 ブロック内で、以下を使用:
- 下: 1,2
- 左: 4,5
- 右: 7,8
- 上: 10,11

### 実装内容
- `player_tiles` を ゆうしゃシートへ差し替え
- `mage_field_tiles`
- `priest_field_tiles`
- `fourth_party_field_tiles`
  を追加（将来の仲間表示や切替用の下準備）

### 色変換ルール
- 外周からつながる白背景 → 透明
- 純白 → 白
- 純黒 → 黒
- 黒でも白でもない色 → 中間色


## rpg102_party_bank0_trim

rpg100/rpg101で追加した仲間ロスター処理により Bank 0 が大きく増えたため、
`party_runtime.c/h` を追加して、仲間データ・生存判定・敵攻撃対象選択・予備仲間入れ替えAPIを bank 7 へ分離。

目的:
- rpg100 の Bank 0 overflow を解消する
- rpg101 のフィールドスプライト差し替えは維持する
- そうりょ/まほう/予備3名のデータ実装は維持する


## rpg103_bank0_thunk_trim

rpg102 は Bank 0 の越境が `0x4000 -> 0x40d8` まで縮小したが、まだ romusage の危険判定が残った。
原因候補は、まだ既存処理から呼んでいない将来用モジュール
`game_flags.c`, `quest.c`, `inventory.c` の多数の `BANKED` 公開関数が、SDCC/GBDK の banked call trampoline を `_HOME` に生成していること。

今回の対応:
- `game_flags` / `quest` / `inventory` は、まだ main.c から未接続の将来用モジュールなので、関数の `BANKED` 指定を一旦外す。
- `#pragma bank 7` は維持し、実装本体は Bank 7 側に置いたままにする。
- 既存戦闘・仲間処理で使っている `party_runtime.c` の `BANKED` API は維持する。

目的:
- Bank 0 の `_HOME` trampoline を削る
- 既存挙動を変えずに romusage warning を消す


## rpg104_enemy_sprite_loop_trim

目的: rpg103で残った Bank 0 overflow (`_HOME 0x4000 -> 0x40d8`) を削るため、main.c内の重複した戦闘OBJ再配置処理をループ化して縮小。

修正内容:
- `show_battle_party_icons()` と `battle_reposition_party_icons_only()` の重複を `battle_place_party_icons()` に集約。
- `show_battle_enemy_sprites()` と `battle_reposition_enemy_sprites_only()` の重複分岐を `battle_refresh_enemy_sprites_compact()` に集約。
- 敵1体/2体/3体のX座標は従来と同じ: 72 / 56,96 / 36,76,116。
- 既存の仲間・スプライト・Banked module構成は維持。


## rpg105_prototype_bank0_fix

rpg104 のビルドログで出ていた `hide_battle_enemy_sprites()` の implicit declaration を修正。

原因:
- `battle_refresh_enemy_sprites_compact()` が `hide_battle_enemy_sprites()` のプロトタイプ宣言より前に定義されていた。
- SDCC が暗黙宣言として扱い、warning 112 / warning 84 が出ていた。

対応:
- `hide_battle_enemy_sprites()` のプロトタイプを `battle_refresh_enemy_sprites_compact()` より前へ移動。
- 後ろ側の重複宣言を削除。

期待:
- implicit declaration warning が消える。
- rpg104 の `_HOME 0x4000 -> 0x4001` という残り2byte相当の越境も、暗黙宣言由来なら解消する可能性がある。


## rpg106_actor_init_bank_trim

rpg105 で残っていた Bank 0 overflow warning を解消するため、
`actors[]` の大きな静的初期化をやめ、BSS配置 + banked runtime初期化へ変更。

修正:
- `static Actor actors[] = {...}` を `Actor actors[3u];` に変更
- `actor_runtime.c/h` を追加
- `actor_runtime_init_actors()` を bank 7 へ配置
- `init_game()` で actor 初期化を呼び出し
- `calc_heal_amount()` の到達不能 clamp を削除

狙い:
- `_INITIALIZER` / `_INITIALIZED` の 88 bytes を削減
- rpg105 の `_HOME 0x4000 -> 0x4001` と `_INITIALIZER` warning をまとめて解消する


## rpg107_bank0_final_trim

rpg106 で残っていた Bank 0 overflow warning をさらに削るための最小整理版。

背景:
- rpg106 では `_INITIALIZER` が 10 bytes まで縮小したが、まだ romusage の dangerous warning が残った。
- 残りは明示初期化された小さな static 変数群と、未使用関数による Bank 0 コードが主因と判断。

修正:
- `audio.c` の static UINT8 明示初期化をBSS化し、`audio_init()` で初期化
- `dialogue.c` の `dialogue_active = 0u` をBSS化
- `jpfont.c` の `jp_cache_clock = 1u`, `jp_next_tile = JP_TILE_BASE` をBSS化し、既存の `jp_reset_cache()` 初期化に一本化
- `main.c` の未使用関数を削除
  - `min_u8()`
  - `put_hp_pair()`
  - `battle_update_party_hp_dirty()`
  - `battle_update_party_mp_dirty()`

期待:
- `_INITIALIZER` / `_INITIALIZED` を0に近づける
- main.c側の数十byte程度のコード削減で、rpg106 の `_HOME 0x4000 -> 0x4008` を吸収する


## rpg109_manual_party_control

rpg108の自動仲間行動ではなく、アクティブ3人を順番にコマンド選択できるようにした版。

仕様:
- 1ターン内で `ゆうしゃ -> そうりょ -> まほう` の順にコマンド選択
- 戦闘ステータス欄の名前左に `>` を出し、現在の操作対象を示す
- こうげき: 現在操作中の仲間で攻撃
- とくぎ:
  - ゆうしゃ: 既存の特技
  - そうりょ: 回復
  - まほう: 火力攻撃
- かいふく: 現在操作中の仲間自身を回復
- にげる: 現在操作中の仲間の素早さで逃走判定
- 全員行動後に敵ターンへ進む

Bank 0対策:
- rpg108の自動行動処理は採用せず、rpg107安定版から最小追加
- `party_battle_op()` にMP消費/HP回復を集約し、BANKED入口を増やしすぎない


## rpg110_bank0_turn_trim

rpg109_manual_party_control の機能を維持しつつ、Bank 0 overflow を消すための小型化版。

rpg109 の残り:
- `_HOME (0x4000 -> 0x4012)`
- `_INITIALIZER (0x4013 -> 0x4013)`
- `_GSINIT (0x4014 -> 0x401f)`
- `_GSFINAL (0x4020 -> 0x4020)`

修正:
- `battle_current_consume_mp()` から `player_battle.mp` への二重同期を削除
- 敵攻撃時の `player_battle.hp` への二重同期を削除

理由:
- rpg109 では現在操作中の仲間ステータスは `party_runtime` 側から取得する
- UI表示も `party_get_active_hp/mp()` を参照している
- `party_damage_active()` と `party_battle_op()` が戦闘中HP/MPの実体になっているため、旧単独ゆうしゃ用の `player_battle` ミラー更新は不要
- 機能を削らずにHOMEコードを小さくする

## rpg186_official_misaki_rpg_kanji

rpg185で簡易追加していたRPG頻出漢字について、JIS X 0208公式PNG版の美咲ゴシック字形を抽出し、`misakiUTF16FontData.h` の該当7バイト字形へ反映しました。

- `main.c` 未変更
- Bank 0増加なし
- フォント配置は引き続き Bank 6
- フォント数とテーブル順序は rpg185 と同じ
- 元の美咲フォントPNG本体は同梱していません


## rpg187_font_usable_chars_handover

rpg186 のフォント構成を前提に、使用可能文字一覧をドキュメント化しました。

追加ファイル:
- `FONT_USABLE_CHARS_rpg187.md`
- `FONT_USABLE_CHARS_rpg187.txt`
- `CHECK_rpg187_font_usable_chars_handover.md`

整理内容:
- 現行フォント総数: 1542 文字
- 使用可能漢字: 1095 文字
  - もともとの教育版ベース漢字: 1007 文字
  - rpg185/rpg186で追加したRPG頻出漢字: 88 文字
- コード本体・フォント本体・`main.c` は未変更
- Bank 0増加なし

運用メモ:
- アイテム名・装備名・技名を増やす際は `FONT_USABLE_CHARS_rpg187.md` を確認する。
- 一覧にない漢字を使うと文字化けするため、追加する場合は Bank 6 側のフォントデータ更新として扱う。
- 現行テーブルには `℃№℡Å`、ローマ数字、丸数字、数学記号などが残っているため、不要なら次回削減候補。


## rpg188_ui_fast_pages_battle_items_font_fullwidth
- 全角英数字を復旧。FONT_USABLE_CHARS_rpg188.md/txt を追加。
- 十字キー押しっぱなし移動、道具/装備候補/パラメーターのページ矢印、戦闘中道具選択、逃げる復活を実装。
- パラメーターは基本/熟練度/特技一覧の3ページ。キャラスプライト表示を追加。
- Bank 7保護のため battle_data_bank.c を Bank 10へ移動。main.cは大型コメント削除でソースサイズを縮小。

## rpg189_bank8_overlap_fix 追記

rpg188/rpg183系で `inventory.c` と `ui_icons.c` がともに Bank 8 を使用していたため、実GBDKリンク時に `Multiple write at 0x20000` および `Possible overflow from Bank 0 into Bank 8` が出る問題に対応した。

- `ui_icons.c`: Bank 8 → Bank 1
- `inventory.c`: Bank 8 のまま
- `main.c`: 未変更
- Bank 0増加なし



## rpg199追記: Bank重なり・Overflowの事前チェック方針

コンパイル前だけで最終的なBank重なり・Overflowを完全確定することはできない。  
ただし、`#pragma bank` 配置、`bank_ids.h`、危険Bank使用、Cソースサイズ合計、前回mapを使った参考チェックは可能。

追加ファイル:

- `tools/prebank_static_check.py`
- `docs/design/11_bank_precheck_policy.md`
- `CHECK_rpg199_bank_precheck_handover.md`

推奨手順:

```bash
python3 tools/prebank_static_check.py --strict
./build.sh
/opt/gbdk/bin/romusage rpgXXX.map -a -g -B
/opt/gbdk/bin/romusage rpgXXX.map -q -E
```

以下が出た場合、GBファイルが生成されても失敗扱いにする。

```text
Write from one bank spans into the next
Multiple write
Possible overflow
Overflow by
Area _CODE_N extends past end of memory region
```

特にBank 8は過去に `0x20000` 起点のMultiple writeが出ているため、当面は新規配置先として避ける。


# rpg202_itil_tower_quiz

## 追加内容
- 100階ある「ITILの塔」を追加。
- フィールド下部中央 `(8,14)` から塔へ入る。
- 入場時に中央UIで `ITILの塔◯階` を表示。
- 各階に3択ITIL問題NPCを配置。
  - NPC位置: `(8,8)`
  - 正解すると右出口へ進める。
  - 不正解なら即退場。
- 各階の右出口を実装。
  - 右出口位置: `(14,2)`
  - 100階未満: 次の階へ進む。
  - 100階: ランダム報酬を獲得して退場。
- ランダム報酬を追加。
  - `ITILの剣`: 攻撃力 +100
  - `ITILの鎧`: 防御力 +100
  - `ITILの紋章`: とくぎ +100

## 実装方針
- Bank 0を大きく増やさないため、問題UI・階層状態・報酬付与は `itil_tower_runtime.c` に分離。
- `itil_tower_runtime.c` は Bank 10。
- 100問をベタ持ちせず、12問を階層ごとにローテーションする方式。
- マップは既存ダンジョンマップを塔エリアでも流用し、専用大規模マップ配列は追加しない。
- `MAP_GFX_TILE_COUNT` は増やしていない。

## 追加/変更ファイル
- `itil_tower_runtime.c`
- `itil_tower_runtime.h`
- `main.c`
- `field_feature_runtime.c/h`
- `map_data.h`
- `map_data_runtime.c`
- `map_data_bank.c`
- `actor_runtime.c`
- `inventory.h/c`
- `party_runtime.c`
- `ui_icons.c`
- `save_bridge_runtime.c`
- `build.sh`
- `bank_ids.h`

## 注意
- この環境ではGBDK実ビルド未実行。
- Bank 10にクイズ処理を追加したため、ビルド後は `romusage` でBank 10 overflowを確認すること。
- Bank 8は引き続き新規配置先として使わない。

## rpg203追記: ITILの塔 100問化

- 既存の12問ローテーションを廃止。
- 1階から100階まで、それぞれ別のITIL問題を出すように変更。
- `itil_tower_runtime.c` を Bank 16 へ移動。
- ROMバンク数を `-Wl-yo32` に拡張。
- Bank 0 には問題文・選択肢を置かない。
- 問題一覧は `docs/future/ITIL_TOWER_100_QUESTIONS.md` に格納。


## rpg204追記: Bank0イベント遷移分離

rpg202/rpg203系で `Possible overflow from Bank 0 into Bank 1` が出たため、`apply_map_event_transition()` の本体をBank 17の `map_event_runtime.c` へ分離。  
`main.c` 側は薄いラッパーだけにし、Bank 0の容量を削減する。  
ITILの塔100問化と `-Wl-yo32` は維持。


## rpg205追記: Bank0ランダム遭遇判定分離

rpg204でも `Possible overflow from Bank 0 into Bank 1` が残ったため、`field_random_encounter_should_start()` の本体ロジックを `field_feature_runtime.c` へ分離。  
`main.c` 側は薄いラッパーにし、Bank 0を追加削減する。


## rpg206追記: Bank0音楽トラック判定分離

rpg205でも `Possible overflow from Bank 0 into Bank 1` が約0x30 bytes残ったため、
`current_area_music_track()` を `field_feature_runtime.c` の `field_feature_music_track(area)` へ分離。
Bank 0には音楽トラック判定本体を置かない。


## rpg207追記: Bank0エリア判定関数削除

rpg206でも `Possible overflow from Bank 0 into Bank 1` が約0x2A bytes残ったため、
`current_area_is_dangerous()` と `current_area_is_town_like()` を削除。  
`draw_object_map()` では `area_dangerous` を1回だけ計算して使い回す。


## rpg208追記: Bank0小ラッパー削除

rpg207でも `Possible overflow from Bank 0 into Bank 1` が約0x22 bytes残ったため、
`battle_reposition_party_icons_only()` / `player_skill()` / `player_use_skill()` / `reload_field_bg_tiles()` を削除し、直接呼び出しへ置換。


## rpg209追記: Bank0遷移ラッパー削除

rpg208でも `Possible overflow from Bank 0 into Bank 1` が約0x16 bytes残ったため、
`apply_map_event_transition()` の薄いラッパーを削除し、
呼び出し元から `map_event_runtime_apply_transition(event_id, current_area)` を直接呼ぶように変更。


## rpg210追記: Bank0単発ラッパー削除

rpg209でも `Possible overflow from Bank 0 into Bank 1` が約0x1C bytes残ったため、
`enter_random_battle()` と `open_save_point_menu()` を削除し、単発呼び出し元へ直接展開した。


## rpg211追記: Bank0 enter_battle単発ラッパー削除

rpg210でも `Possible overflow from Bank 0 into Bank 1` が約0x18 bytes残ったため、
`enter_battle(enemy_index)` を削除し、呼び出し元へ直接展開した。


## rpg213追記: ITIL問題文Bank18分離

ITILの塔100問の問題文テーブルを `itil_tower_runtime.c` から分離し、`itil_quiz_bank.c` Bank18へ移動。  
`itil_tower_runtime.c` はBank16で塔の状態管理とUI制御を担当し、問題文・選択肢・正解判定はBank18の `itil_quiz_draw()` / `itil_quiz_is_correct()` / `itil_quiz_count()` 経由で扱う。  
なお、Bank0 overflowの直接原因は問題文ではなくmain.c側の処理肥大化のため、同時にbattle系の小ラッパーも削除。


## rpg214追記: Bank0戦闘ロードラッパー削除

rpg213でも `Possible overflow from Bank 0 into Bank 1` が約0x10 bytes残ったため、
`show_battle_party_icons()` と `load_battle_enemy_sprite_data()` を削除し、呼び出し元へ直接展開した。
ITIL問題文はすでにBank18に分離済みなので、今回のBank0 overflow原因ではない。


## rpg215追記: Bank0回復泉処理分離

rpg214でも `Possible overflow from Bank 0 into Bank 1` が約6bytes残ったため、
`activate_heal_spring()` を削除し、回復処理本体を `field_feature_runtime.c` Bank5 の
`field_feature_activate_heal_spring_runtime()` へ移動。  
`draw_all_actors()` はmain.c側に残す。


## rpg216追記: Bank0戦闘小ラッパー削除

rpg215でも `Possible overflow from Bank 0 into Bank 1` が約10bytes残ったため、
`battle_start_skill_select()` / `battle_start_ally_target_select()` / `player_defend()` /
`battle_current_consume_mp()` を削除し、呼び出し元へ直接展開した。  
あわせて `itil_quiz_bank.c` の到達不能分岐を削除。


## rpg217追記: Bank0単発初期化ロード関数削除

rpg216でも `Possible overflow from Bank 0 into Bank 1` が約0x0E bytes残ったため、
`init_player_skills()` と `load_battle_party_icon_sprite_data()` を削除し、呼び出し元へ直接展開した。


## rpg218追記: Bank0カメラ最大座標関数マクロ化

rpg217でも `Possible overflow from Bank 0 into Bank 1` が約5bytes残ったため、
`camera_max_px_x()` / `camera_max_px_y()` を削除し、`CAMERA_MAX_PX_X` / `CAMERA_MAX_PX_Y` マクロへ置換。
