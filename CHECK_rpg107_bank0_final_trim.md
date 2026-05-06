# CHECK rpg107_bank0_final_trim

## 目的
rpg106_actor_init_bank_trim で残った Bank 0 overflow warning を消すための最終小型化。

## rpg106 の残り
```text
_HOME        (0x4000 -> 0x4008)
_INITIALIZER (0x4009 -> 0x4012)
_GSINIT      (0x4013 -> 0x401e)
_GSFINAL     (0x401f -> 0x401f)
```

## 修正内容

### audio.c
- 明示初期化された static UINT8 群をBSS化
- `audio_init()` で明示初期化
- `audio_battle_noise_flip` も `audio_init()` で0初期化

### dialogue.c
- `dialogue_active = 0u` をBSS化

### jpfont.c
- `jp_cache_clock = 1u`
- `jp_next_tile = JP_TILE_BASE`
をBSS化
- 既存の `jp_reset_cache()` 初期化に一本化

### main.c
未使用関数とプロトタイプを削除:
- `min_u8()`
- `put_hp_pair()`
- `battle_update_party_hp_dirty()`
- `battle_update_party_mp_dirty()`

## 維持している内容
- rpg106のactor_runtime分離
- rpg101のフィールドプレイヤースプライト差し替え
- rpg100以降の仲間ロスター
- rpg104の敵スプライト再配置共通化
- audio/dialogue/jpfontの初期化順序

## 確認観点
- romusage の Bank 0 overflow warning が消えるか
- `_INITIALIZER` が消える、または大きく減るか
- フィールドBGM/戦闘BGMが鳴るか
- 会話ウィンドウが表示されるか
- 日本語フォントキャッシュが正常に初期化されるか
- 戦闘UIと仲間表示が崩れないか
