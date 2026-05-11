# rpg218_bank0_camera_max_macro_trim

## 対応内容

rpg217でもBank 0 overflowが最後に約5 bytes残っていたため、Bank 0内の小さいカメラ最大座標関数を削除。

今回のログ:

```text
Multiple write of 5 bytes at 0x4000 -> 0x4004
Possible overflow from Bank 0 into Bank 1
```

## 修正方針

小さい定数計算関数を関数として持つのをやめ、定数マクロへ変更。

削除:
- `camera_max_px_x()`
- `camera_max_px_y()`

追加:
```c
#define CAMERA_MAX_PX_X ((UINT8)((MAP16_W * TILE16_PX > SCREEN_PX_W) ? (MAP16_W * TILE16_PX - SCREEN_PX_W) : 0u))
#define CAMERA_MAX_PX_Y ((UINT8)((MAP16_H * TILE16_PX > SCREEN_PX_H) ? (MAP16_H * TILE16_PX - SCREEN_PX_H) : 0u))
```

置換:
- `camera_max_px_x()` → `CAMERA_MAX_PX_X`
- `camera_max_px_y()` → `CAMERA_MAX_PX_Y`

## 狙い

rpg217時点のBank0超過は約5bytesなので、関数本体とCALL/RET相当を消して収める。

## 維持している内容

- rpg217の単発初期化ロード関数削除
- rpg216の戦闘小ラッパー削除
- rpg215の回復泉Bank5分離
- ITILの塔100問化
- `itil_tower_runtime.c` Bank 16
- `itil_quiz_bank.c` Bank 18
- `map_event_runtime.c` Bank 17
- `-Wl-yo32`

## 次の確認

以下が消えるか確認。

```text
Possible overflow from Bank 0 into Bank 1
Multiple write
Write from one bank spans into the next
Overflow by
```

まだ残る場合は、次は `battle_reset_bg_origin()` か `battle_hide_command_cursor_obj()` を見直す。
