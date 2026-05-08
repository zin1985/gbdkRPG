# CHECK rpg137_dungeon_npc_placement

## 変更内容
- `actor_runtime_apply_area_npcs(UINT8 area) BANKED` を追加。
- 町・港町・洞窟・古代遺跡ごとにNPC配置を切替。
- 洞窟/遺跡にそれぞれ2体のNPCを配置。
- NPC用メッセージを `messages_bank.c` に追加。
- 敵復活プロンプトを町系エリア限定に調整。
- 出力ROM名を `rpg137_dungeon_npc_placement.gb` に変更。

## 確認ポイント
1. ビルド時に Bank 0 overflow が出ないこと。
2. 洞窟に入って `(4,5)` と `(10,11)` 周辺にNPCが表示されること。
3. 古代遺跡に入って `(2,4)` と `(10,12)` 周辺にNPCが表示されること。
4. ダンジョンNPCに話しかけた際、敵復活プロンプトが出ないこと。
5. フィールドではNPC/旧敵テンプレートが表示されないこと。
