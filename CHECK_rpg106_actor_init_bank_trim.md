# CHECK rpg106_actor_init_bank_trim

## 目的
rpg105_prototype_bank0_fix で残った Bank 0 overflow warning を解消する。

## rpg105 の状態
```text
_HOME        (0x4000 -> 0x4001)
_INITIALIZER (0x4002 -> 0x4059)
_GSINIT      (0x405a -> 0x4065)
_GSFINAL     (0x4066 -> 0x4066)
```

## 原因仮説
`actors[]` の静的初期化により `_INITIALIZER` / `_INITIALIZED` が 88 bytes 残っていた。
rpg105では `_HOME` 自体は残り2byte相当まで縮んでいたが、initializer領域まで含めるとBank 0判定が消えなかった。

## 修正内容
- `main.c`
  - `static Actor actors[] = {...}` を `Actor actors[3u];` に変更
  - `init_game()` で `actor_runtime_init_actors()` を呼ぶ
  - 到達不能だった `calc_heal_amount()` の999上限チェックを削除

- `actor_runtime.c/h`
  - actor初期値設定を bank 7 へ移動
  - 既存のNPC/敵/テストactorの初期配置・sprite/tile/message設定は維持

## 維持している内容
- NPC 1体
- 敵actor 1体
- テスト会話actor
- rpg101のフィールドスプライト差し替え
- rpg100以降の仲間ロスター
- rpg104の敵スプライト再配置共通化

## 確認観点
- romusage の Bank 0 overflow warning が消えるか
- フィールド上のNPC/テスト会話actorが表示されるか
- 敵actorの復活/撃破処理が壊れていないか
- ランダムエンカウント戦闘が動くか
- 戦闘UIが崩れていないか
