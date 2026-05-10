# rpg177_sram_evap_debug

目的: 実機カートリッジで「セーブ直後は読めるが10秒OFFで消える」現象を切り分けるため、SRAM保持デバッグ画面を追加。

## 操作
タイトル画面で SELECT を押すと SRAM DEBUG を開く。

- A: SRAM末尾付近(0xA000+0x1F00)へ独立テストパターンを書き込み
- START: 電源ONのまま10秒待って再読込
- SELECT: 再読込
- B: タイトルへ戻る

## 判定
1. タイトルでSELECT→A WRITE TEST
2. START WAIT10で電源ON中に保持されるか確認
3. 電源OFFして10秒待つ
4. 再起動→タイトルでSELECT

- 電源ON中の10秒はOK、電源OFF後だけLOST: 電池バックアップ経路/SRAM VCC/ダイオード/ホルダー/CE制御が濃厚
- 電源ON中の10秒でもLOST: ソフト側、SRAM enable、アドレス、チップ、MBC互換問題が濃厚
- 通常セーブだけ消えてSRAM DEBUGはOK: SaveSnapshot/checksum/スロット処理が濃厚

## 実装方針
- main.c未変更
- save_runtime.c(Bank 5)のみ変更
- Bank 0は増やさない
- 通常セーブ領域とは別の0x1F00を使用
