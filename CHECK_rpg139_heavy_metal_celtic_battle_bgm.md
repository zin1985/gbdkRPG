# rpg139_heavy_metal_celtic_battle_bgm

## 目的
アップロードされた `heavy_metal_celtic_battle_bgm.c/.h` を戦闘BGMとして組み込み、既存の `audio_play_music(AUDIO_TRACK_BATTLE)` 呼び出しから鳴るようにした。

## 修正したファイル

### audio.c
- `heavy_metal_celtic_battle_bgm.h` を include。
- `AUDIO_TRACK_BATTLE` の再生時だけ、既存の簡易BGMではなく `heavy_metal_celtic_battle_bgm_init()` を呼ぶように変更。
- `audio_update()` で戦闘中は `heavy_metal_celtic_battle_bgm_update()` を毎フレーム呼ぶように変更。
- 戦闘終了やフィールド/町BGMへ切り替える時に `heavy_metal_celtic_battle_bgm_stop()` を呼び、CH1/CH2/CH3/CH4 を止めるようにした。
- 旧ミニ戦闘BGMテーブルを削除し、Bank 0増加を避けた。

### heavy_metal_celtic_battle_bgm.c
- 新規追加。
- `#pragma bank 15` を追加し、重いBGMデータをBank 0に置かないようにした。
- 公開関数を `BANKED` 化。
- 4ch使用。CH1=リード、CH2=ギター風パルス、CH3=低音、CH4=ドラム。

### heavy_metal_celtic_battle_bgm.h
- 新規追加。
- `BANKED` 宣言を追加。

### build.sh / Makefile / build.bat
- 出力名を `rpg139_heavy_metal_celtic_battle_bgm.gb` に変更。
- `heavy_metal_celtic_battle_bgm.c` をビルド対象に追加。
- build.bat は現行構成に合わせて `field_feature_runtime.c` と `ROM_BANKS=16` も反映。

## 修正していないファイル
- main.c は未変更。既存の `enter_battle_screen()` 内の `audio_play_music(AUDIO_TRACK_BATTLE)` をそのまま利用。
- 戦闘ロジック、敵BG表示、NPC、エンカウント、装備、ステータス、クエストは未変更。
- 既存のフィールドBGM/町BGMは維持。

## 注意点
- このBGMは全4chを使用するため、戦闘中SEと競合しやすい。
- 重要SEを鳴らす場合は、CH4ドラムを一時的に抑制するか、SE優先時だけBGM側のCH4を止める設計が必要。
- 今回はBGM差し替え中心で、SE優先制御までは未実装。

## ビルド確認
この環境では `/opt/gbdk/bin/lcc` が存在しないため、実ビルドは未実行。
ローカルのGBDK環境で以下を実行して確認すること。

```sh
./build.sh
```

または

```sh
make
```
