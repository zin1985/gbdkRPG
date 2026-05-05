# CHECK: rpg017

## 起動確認

- [ ] 起動して白画面にならない
- [ ] フィールドが表示される
- [ ] プレイヤーが移動できる
- [ ] 尖った山が通れない
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] 戦闘画面に入れる

## 変更確認

- [ ] ステータス画面から位置/カメラのデバッグ表示が消えている
- [ ] ステータス画面の基本ステータス表示は残っている
- [ ] スプライト追加はまだ入っていない

## ビルド確認

- [ ] build.bat が `-Wl-m -Wl-j` を含む
- [ ] build.sh が `-Wl-m -Wl-j` を含む
- [ ] Makefile が `-Wl-m -Wl-j` を含む
- [ ] `.map` / `.sym` / `.noi` の出力を確認できる

## 非変更確認

- [ ] MAP_GFX_TILE_COUNT は 13u のまま
- [ ] 新規BGタイルなし
- [ ] 新規object kindなし
- [ ] 新規actor/NPCなし
- [ ] VRAMロード経路変更なし
- [ ] init_map_sprites() に装飾スプライト追加なし


## CHECK: rpg022 BG forest overlay

- [ ] 起動して白画面にならない
- [ ] フィールドが表示される
- [ ] フィールドの指定座標に森マーカー風BGが1つ表示される
- [ ] 森マーカー上を歩ける
- [ ] 尖った山は通れないまま
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] 戦闘画面に入れる
- [ ] MAP_GFX_TILE_COUNTが13uのまま
- [ ] 新規object kindを追加していない
- [ ] スプライト/OAMを追加していない


## CHECK: rpg024 town marker + forest BG

- [ ] 起動して白画面にならない
- [ ] フィールドの町マーカーが以前の町アイコンとして残っている
- [ ] 森BGが町マーカーと別の濃い森として表示される
- [ ] 森BGの上を歩ける
- [ ] 尖った山は通れないまま
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] 戦闘画面に入れる
- [ ] 新規object kindを追加していない
- [ ] collision16_mapを変更していない
- [ ] スプライト/OAMを追加していない


## CHECK: rpg029 baseline

- [ ] 起動して白画面にならない
- [ ] フィールドが表示される
- [ ] 町マーカーが表示される
- [ ] 森BGが表示される
- [ ] 森の上を歩ける
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] 戦闘に入れる
- [ ] main.c の `GBDK RPG SAFE MODIFICATION CHECKPOINT` コメントを確認した


## CHECK: rpg030 graphics architecture

- [ ] 起動して白画面にならない
- [ ] フィールドが表示される
- [ ] 町マーカーが表示される
- [ ] 森BGが表示される
- [ ] プレイヤーが移動できる
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] 戦闘に入れる
- [ ] GRAPHICS_ARCHITECTURE_030.md が含まれている
- [ ] SAFE_GRAPHICS_CHECKPOINT_030.md が含まれている
- [ ] PROGRAM_FLOW_DEBUG_030.md が含まれている


## CHECK: rpg046 inactive actor slot

- [ ] 起動して白画面にならない
- [ ] 見た目がrpg030と同じ
- [ ] 新しいスプライトが表示されない
- [ ] プレイヤーが表示される
- [ ] NPCが表示される
- [ ] 敵が表示される
- [ ] 移動できる
- [ ] NPCと会話できる
- [ ] 敵と戦闘できる
- [ ] 戦闘後に戻れる
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg047 visible actor slot

- [ ] 起動して白画面にならない
- [ ] 追加NPC風スプライトが表示される
- [ ] 既存NPCが表示される
- [ ] 敵が表示される
- [ ] プレイヤーが表示される
- [ ] 追加スプライトをすり抜けられる
- [ ] 追加スプライトに話しかけても通常会話が暴発しない
- [ ] NPCと会話できる
- [ ] 敵と戦闘できる
- [ ] 戦闘後に戻れる
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg048b talkable data-only

- [ ] 起動して白画面にならない
- [ ] 追加NPC風スプライトが表示される
- [ ] 追加NPC風スプライトをすり抜けられる
- [ ] 追加NPC風スプライトに話しかけられる
- [ ] 既存NPCと会話できる
- [ ] 敵と戦闘できる
- [ ] 戦闘後に戻れる
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg049 safe event policy

- [ ] 起動して白画面にならない
- [ ] rpg048bと同じ見た目で表示される
- [ ] 追加NPC風スプライトが表示される
- [ ] 追加NPC風スプライトに話しかけられる
- [ ] 既存NPCと会話できる
- [ ] 敵と戦闘できる
- [ ] 戦闘後に戻れる
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] build.logにASlink warningがない
- [ ] build.logにoverflow/relocation/segment overlapがない


## CHECK: rpg051 build romusage check

- [ ] ビルドが成功する
- [ ] `build.log` が生成される
- [ ] `.map` が生成される
- [ ] `.noi` が生成される
- [ ] `.sym` が生成される
- [ ] `build.log` に `ASlink-Warning` がない
- [ ] `build.log` に `Write from one bank spans into the next` がない
- [ ] `build.log` に `overflow` がない
- [ ] `build.log` に `relocation` がない
- [ ] `build.log` に `segment overlap` がない
- [ ] `romusage` があれば結果が出力される
- [ ] 起動して白画面にならない
- [ ] rpg049と同じ挙動で動く


## CHECK: rpg055 banked messages

- [ ] ビルドが成功する
- [ ] `build.log` が生成される
- [ ] `build.log` に `Write from one bank spans into the next` がない
- [ ] `build.log` に `ASlink-Warning` がない
- [ ] `build.log` に `overflow` がない
- [ ] `romusage` 結果が出る、または未導入警告が出る
- [ ] 起動して白画面にならない
- [ ] 既存NPCと会話できる
- [ ] 追加NPCと会話できる
- [ ] 追加NPCの短文 `みちを / みてね。` が出る
- [ ] 敵と戦闘できる
- [ ] 戦闘後に戻れる
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg056 graphics bank fix

- [ ] ビルドが成功する
- [ ] `build.log` に `Write from one bank spans into the next` がない
- [ ] `build.log` に `Possible overflow` がない
- [ ] `build.log` に `Multiple write` がない
- [ ] `build.log` に `ASlink-Warning` がない
- [ ] 起動して白画面にならない
- [ ] BGタイルが正常表示される
- [ ] 主人公/NPC/敵スプライトが正常表示される
- [ ] 既存NPCと会話できる
- [ ] 追加NPCと会話できる
- [ ] 追加NPCの短文 `みちを / みてね。` が出る
- [ ] 敵と戦闘できる
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg057 build warning check fix

- [ ] ビルドが成功する
- [ ] `grep: build.log: input file is also the output` が出ない
- [ ] `precheck_output.log` が生成される
- [ ] `compile_output.log` が生成される
- [ ] `build.log` が生成される
- [ ] `compile_output.log` に `Write from one bank spans into the next` がない
- [ ] `compile_output.log` に `Possible overflow` がない
- [ ] `compile_output.log` に `Multiple write` がない
- [ ] 起動して白画面にならない
- [ ] BGタイルが正常表示される
- [ ] 主人公/NPC/敵スプライトが正常表示される
- [ ] 会話できる
- [ ] 戦闘できる


## CHECK: rpg058 map data bank split

- [ ] ビルドが成功する
- [ ] `compile_output.log` に危険警告がない
- [ ] `romusage_output.log` に `Possible overflow beyond Bank 0` がない
- [ ] `romusage_output.log` に `Possible overflow` がない
- [ ] `romusage_output.log` に `Multiple write` がない
- [ ] 起動して白画面にならない
- [ ] フィールドBGが表示される
- [ ] 町BGが表示される
- [ ] 壁判定が効く
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] NPCと会話できる
- [ ] 敵と戦闘できる


## CHECK: rpg059 no console stdio

- [ ] ビルドが成功する
- [ ] `map_data.h redefined` warning が消えている
- [ ] `compile_output.log` に危険警告がない
- [ ] `romusage_output.log` に `Possible overflow beyond Bank 0` がない
- [ ] 起動して白画面にならない
- [ ] メニューが表示される
- [ ] ステータス画面の数値が表示される
- [ ] 戦闘画面のHPが表示される
- [ ] 戦闘コマンドカーソルが表示される
- [ ] 町に入れる
- [ ] 町から戻れる
- [ ] NPCと会話できる


## CHECK: rpg060 helper definition fix

- [ ] `_put_u8` undefined が出ない
- [ ] `_screen_clear` undefined が出ない
- [ ] `_put_ascii` undefined が出ない
- [ ] `_put_cursor` undefined が出ない
- [ ] `_put_hp_pair` undefined が出ない
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log に `Possible overflow beyond Bank 0` がない
- [ ] 起動して白画面にならない
- [ ] メニューが表示される
- [ ] 戦闘画面が表示される


## CHECK: rpg061 bank jpfont misaki

- [ ] ビルドが成功する
- [ ] `compile_output.log` に危険警告がない
- [ ] `romusage_output.log` に `Possible overflow beyond Bank 0` がない
- [ ] 起動して白画面にならない
- [ ] フィールドが表示される
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] ステータス数値が表示される
- [ ] 戦闘画面文字が表示される
- [ ] 町に入れる
- [ ] 町から戻れる


## CHECK: rpg062 misaki bankref include fix

- [ ] `misakiUTF16.c: return type of function omitted` が出ない
- [ ] `misakiUTF16.c: syntax error` が出ない
- [ ] ビルドが成功する
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log に `Possible overflow beyond Bank 0` がない
- [ ] 起動して白画面にならない
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] 戦闘文字が表示される


## CHECK: rpg063 misaki separate bank

- [ ] bank 5 -> bank 6 の `Write from one bank spans into the next` が消える
- [ ] `misakiUTF16.c` の関数で undefined global が出ない
- [ ] ビルドが成功する
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log に `Possible overflow beyond Bank 0` がない
- [ ] 起動して白画面にならない
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] 戦闘文字が表示される


## CHECK: rpg064 jpfont unbanked misaki bank

- [ ] ビルドが成功する
- [ ] bank 5 -> 6 の overflow が出ない
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log を確認する
- [ ] 起動して白画面にならない
- [ ] NPC会話を開いてフリーズしない
- [ ] 町移動時のメッセージ/処理でフリーズしない
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] 戦闘文字が表示される


## CHECK: rpg065 fontdata manual bank

- [ ] ビルドが成功する
- [ ] bank overflow warning が出ない
- [ ] 起動して白画面にならない
- [ ] NPC会話を開いてフリーズしない
- [ ] 町移動でフリーズしない
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] 戦闘文字が表示される


## CHECK: rpg066 fontdata gettable fix

- [ ] `Undefined identifier 'fdata'` が出ない
- [ ] ビルドが成功する
- [ ] bank overflow warning が出ない
- [ ] 起動して白画面にならない
- [ ] NPC会話を開いてフリーズしない
- [ ] 町移動でフリーズしない
- [ ] 会話文字が表示される
- [ ] メニュー文字が表示される
- [ ] 戦闘文字が表示される
- [ ] romusage_output.log を確認する


## CHECK: rpg067 area actor visibility

- [ ] ビルドが成功する
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log を確認する
- [ ] 起動して白画面にならない
- [ ] フィールドで敵actorが表示される
- [ ] フィールドでNPC actorが表示されない
- [ ] フィールドで非表示NPCに話しかけてもフリーズしない
- [ ] 町に入れる
- [ ] 町でNPC actorが表示される
- [ ] 町で敵actorが表示されない
- [ ] 町でNPCに話しかけられる
- [ ] 町からフィールドへ戻れる
- [ ] フィールドへ戻ると敵actorが再表示される
- [ ] MAP_GFX_TILE_COUNT が 17u のまま
- [ ] 新規BGタイルが追加されていない
- [ ] 新規actorが追加されていない


## CHECK: rpg068 field random encounter effect

- [ ] ビルドが成功する
- [ ] compile_output.log に危険警告がない
- [ ] romusage_output.log を確認する
- [ ] 起動して白画面にならない
- [ ] フィールドでenemy actorが表示されない
- [ ] フィールドを歩くとランダムで戦闘に入る
- [ ] 町ではランダム戦闘に入らない
- [ ] 町入口/出口イベントでランダム戦闘が割り込まない
- [ ] 戦闘開始時にパレット点滅エフェクトが出る
- [ ] 戦闘後にフィールドへ戻れる
- [ ] 戦闘後すぐ連続エンカウントしにくい
- [ ] 会話がフリーズしない
- [ ] MAP_GFX_TILE_COUNT が 17u のまま
- [ ] 新規actorが追加されていない


## CHECK: rpg069 font cache battle cleanup

- [ ] ビルドが成功する
- [ ] 起動して白画面にならない
- [ ] フィールドを歩くとランダム戦闘に入る
- [ ] 戦闘開始エフェクトが出る
- [ ] 戦闘画面の文字が化けない
- [ ] 開始メッセージ「まものが あらわれた！」が化けない
- [ ] 戦闘メニュー文字が化けない
- [ ] 戦闘後にフィールドへ戻ってBG表示が荒れない
- [ ] 町NPC会話が化けない
- [ ] 町移動メッセージが化けない
- [ ] MAP_GFX_TILE_COUNT が 17u のまま
- [ ] 新規actorが追加されていない
