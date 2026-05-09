# rpg157_bank0_unused_trim

目的: rpg156で残ったBank 0の小さなoverflowを解消するため、main.cに残っていた未使用static関数と未使用prototypeを削除。

削除対象:
- camera_sub_from_px()
- fighter_try_consume_mp()
- battle_alive_count()
- battle_prepare_player_turn_ui()
- battle_current_special_skill()

機能変更なし。Bank 0削減のみ。
