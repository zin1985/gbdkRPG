# rpg171_bgm_bss_bank0_init_trim

目的: rpg170で残っていたBank 0 _GSINIT/_GSFINAL の数バイト超過を解消する。

対応:
- BGM runtime用のstatic変数について、明示的な初期値 `= 0u` / `= 255u` を外し、BSS配置に寄せた。
- 各BGMのinit関数では従来通り位置・tick・playing・last noteを初期化しているため、動作初期化は維持。
- Bank 0のコードや処理機能は増やしていない。

対象:
- heavy_metal_celtic_battle_bgm.c
- sunset_ruins_field_bgm.c
- sunset_strings_adventure_field_bgm.c
- peaceful_balanced_town_bgm.c
- deep_eerie_motif_dungeon_bgm.c
