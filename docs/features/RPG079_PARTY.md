# rpg079_party

## 目的

戦闘中の表示崩れを追加機能ではなく再描画設計の整理で修正する。

## 修正

- 敵攻撃前の白画面対策として、通常ターン中の `DISPLAY_OFF` を使う全画面再描画を停止。
- `update_battle_status()` はLCD OFFなしでWindow退避、BGスクロールリセット、戦闘画面再描画だけを行う。
- カーソル移動では `draw_battle_menu()` を呼ばず、旧カーソルを消して新カーソルだけ表示する。
- コマンド欄は2行だけにし、下段の `とく / かい` 使用回数表示を削除。
- 敵ターン終了後に `battle_reset_screen_for_command(0u)` を呼び、次のコマンド画面を毎ターン正常化する。

## 触っていないもの

- 新規BGタイル
- MAP_GFX_TILE_COUNT
- actor/NPC/object kind
- VRAMロード経路
- jpfont.c / misakiUTF16.c のBANKED化
