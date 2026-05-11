# rpg220_itil_kana_sprite_restore

## 対応内容

### 1. ITIL表記をアイティルへ置換

ユーザー表示上の `ITIL` を `アイティル` に置換。

対象:
- `itil_tower_runtime.c`
  - `ITILの塔` → `アイティルの塔`
  - `ITIL問題` → `アイティル問題`
  - `ITILの塔 制覇` → `アイティルの塔 制覇`
  - `ITILの剣/鎧/紋章` → `アイティルの剣/鎧/紋章`
- `itil_quiz_bank.c`
  - 問題文・選択肢内の `ITIL` を `アイティル` へ置換
- `inventory.c` などのユーザー表示文字列
- `docs/future/ITIL_TOWER_100_QUESTIONS.md`

識別子・定数名・ファイル名は変更していません。
例: `ITEM_ITIL_SWORD` はそのままです。

### 2. ITIL塔UI後にキャラクターが消える問題を修正

原因:
- `itil_clear_ui()` が `HIDE_SPRITES` している
- 階層表示/問題ウィンドウ終了後に `SHOW_SPRITES` が戻らず、メイン側で `draw_all_actors()` してもスプライト表示がOFFのままになる

対応:
- `itil_wait_a()` の最後に `SHOW_SPRITES;` を追加

```c
static void itil_wait_a(void) BANKED {
    audio_waitpad(J_A | J_B | J_START);
    audio_waitpadup();
    SHOW_SPRITES;
}
```

これにより、以下の後にキャラクターが復帰する想定です。

- `アイティルの塔◯階` 表示後
- 問題ウィンドウで正解した後
- 既に合格済みメッセージ表示後
- 不正解退場メッセージ表示後

## 注意

表記変更でBank16/18の文字列容量は少し増える可能性がありますが、Bank0には問題文・塔UI文字列を置いていません。  
最終確認はビルドログとromusageで行ってください。
