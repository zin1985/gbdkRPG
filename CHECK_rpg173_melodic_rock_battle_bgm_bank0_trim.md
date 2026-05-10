# CHECK rpg173_melodic_rock_battle_bgm_bank0_trim

## 目的

- 戦闘曲を `melodic_rock_battle_strings_finale_bgm` に差し替える。
- `rpg172` で残った Bank 0 の `_GSINIT/_GSFINAL` 1 byte 超過をさらに削る。
- SRAM/Battery付きROMヘッダ設定は維持する。

## 変更内容

### 戦闘BGM

`heavy_metal_celtic_battle_bgm.c` の公開API名を維持したまま、中身をアップロード曲へ差し替え。
既存の `audio.c` は `AUDIO_TRACK_BATTLE` から同じAPIを呼び続けるため、呼び出し側の変更は最小。

### Bank 0 trim

`audio.c` の `audio_started` を削除。`main()` で起動時に `audio_init()` 済みのため、重複ガードを外してBank 0を削減。

`dialogue_init()` の `dialogue_active = 0u` を削除。BSS初期値で0になるため、起動時の明示代入を外してBank 0を削減。

### BGM static 初期化

アップロード曲内のBGM runtime変数は明示初期値を外し、`*_init()` 内で初期化する方式に変更。
`_INITIALIZER/_GSINIT` の増加を避けるため。

## 出力名

```text
rpg173_melodic_rock_battle_bgm_bank0_trim.gb
```

## 注意

このチャット環境では `/opt/gbdk/bin/lcc` が無いため、ローカルで `./build.sh` による確認が必要。
