# rpg172_shop_window_dialogue_save_fix

## 修正内容

- 店ウィンドウ描画時に `move_bkg(0,0)` を実行し、カメラスクロール位置に依存しない screen 固定UIとして描画するよう修正。
- 店ウィンドウ表示中は `HIDE_SPRITES` し、店員/主人公/NPCが店UIの前に残らないよう修正。
- 会話ウィンドウ表示中は `HIDE_SPRITES`、閉じる時に `SHOW_SPRITES` するようにして、OBJキャラクターが会話ウィンドウより前に出る問題を抑止。
- ROMヘッダを `MBC5` から `MBC5+SRAM+BATTERY` に変更し、8KB SRAMを有効化。セーブポイントで書いたSRAMがエミュレータ上で battery save として残る設定へ変更。

## 変更ファイル

- `shop_runtime.c`
- `dialogue.c`
- `build.sh`
- `Makefile`

## 注意

この環境には `/opt/gbdk/bin/lcc` が無いためビルド実行は未確認。ローカル環境で `./build.sh` を実行し、romusageのBank 0警告とSRAM保存動作を確認すること。
