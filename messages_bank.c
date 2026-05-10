#pragma bank 2

#include <gb/gb.h>
#include "messages.h"

/*
 * rpg055 banked message data.
 *
 * This file is deliberately placed in ROM bank 2.
 * Keep message strings out of main.c to avoid bank 1 boundary pressure.
 */
BANKREF(messages_bank)

const char msg_common_npc_bank[] =
    "Aで はなしてね。\n"
    "まものの そうだんを\n"
    "しよう。";

const char msg_test_short_bank[] =
    "みちを\n"
    "みてね。";


const char msg_growth_hp_bank[] =
    "ゆうしゃの HPが\n"
    "あがった！";

const char msg_growth_mp_bank[] =
    "ゆうしゃの MPが\n"
    "あがった！";

const char msg_growth_atk_bank[] =
    "ゆうしゃの ちからが\n"
    "あがった！";

const char msg_growth_def_bank[] =
    "ゆうしゃの まもりが\n"
    "あがった！";

const char msg_growth_skill_bank[] =
    "ゆうしゃの とくぎが\n"
    "あがった！";

const char msg_growth_heal_bank[] =
    "ゆうしゃの かいふくが\n"
    "あがった！";

const char msg_growth_agi_bank[] =
    "ゆうしゃの すばやさが\n"
    "あがった！";

const char msg_enemy_still_bank[] =
    "まだ まものは\n"
    "そこに いるよ。";

const char msg_enemy_revived_bank[] =
    "まものが\n"
    "ふっかつした！";

const char msg_leave_as_is_bank[] =
    "そのままに\n"
    "しておこう。";

const char msg_enter_town_bank[] =
    "まちへ\n"
    "はいります。";

const char msg_arrive_town_bank[] =
    "まちに\n"
    "つきました。";

const char msg_back_field_bank[] =
    "フィールドへ\n"
    "もどります。";

const char msg_left_field_bank[] =
    "フィールドに\n"
    "でました。";

const char msg_later_bank[] =
    "また あとで\n"
    "こよう。";

const char msg_stay_town_bank[] =
    "もうすこし\n"
    "まちにいよう。";

const char msg_dark_cave_bank[] =
    "くらい どうくつが\n"
    "つづいている。";

const char msg_enemy_defeated_revive_bank[] =
    "まものは たおした。\n"
    "ふっかつ する？";

const char msg_battle_appear_bank[] =
    "まものが\n"
    "あらわれた！";

const char msg_battle_attack_bank[] =
    "ゆうしゃの こうげき！\n"
    "ダメージ！";

const char msg_battle_low_mp_bank[] =
    "Mがたりない";

const char msg_battle_skill_bank[] =
    "とくぎを つかった！\n"
    "ダメージ！";

const char msg_battle_hp_full_bank[] =
    "Hまんたん";

const char msg_battle_heal_done_bank[] =
    "かいふくした！";

const char msg_battle_escape_ok_bank[] =
    "うまく\n"
    "にげだした！";

const char msg_battle_escape_ng_bank[] =
    "にげられない！";

const char msg_battle_enemy_attack_bank[] =
    "まもの の こうげき！\n"
    "ダメージ！";

const char msg_battle_win_bank[] =
    "まものを\n"
    "たおした！";

const char msg_battle_lose_bank[] =
    "ゆうしゃは\n"
    "たおれた…";

const char msg_prompt_revive_bank[] =
    "まものを ふっかつ?\n"
    "A:はい  B:いいえ";

const char msg_prompt_enter_town_bank[] =
    "まちに はいりますか?\n"
    "A:はい  B:いいえ";

const char msg_prompt_exit_town_bank[] =
    "フィールドへ でますか?\n"
    "A:はい  B:いいえ";


const char msg_prompt_enter_port_bank[] =
    "みなとまちへ\n"
    "いきますか?\n"
    "A:はい  B:いいえ";

const char msg_enter_port_bank[] =
    "みなとまちへ\n"
    "むかいます。";

const char msg_arrive_port_bank[] =
    "しおかぜのまちに\n"
    "つきました。";

const char msg_prompt_enter_dungeon_bank[] =
    "どうくつへ\n"
    "はいりますか?\n"
    "A:はい  B:いいえ";

const char msg_enter_dungeon_bank[] =
    "くらい どうくつへ\n"
    "はいります。";

const char msg_arrive_dungeon_bank[] =
    "じめんが つめたい。\n"
    "まものの けはい。";

const char msg_dungeon_exit_bank[] =
    "どうくつを\n"
    "でました。";

const char msg_prompt_enter_ruins_bank[] =
    "こだい いせきへ\n"
    "はいりますか?\n"
    "A:はい  B:いいえ";

const char msg_enter_ruins_bank[] =
    "こだい いせきへ\n"
    "ふみこみます。";

const char msg_arrive_ruins_bank[] =
    "ふるい いしが\n"
    "しずかに ならぶ。";

const char msg_treasure_found_bank[] =
    "たからばこから\n"
    "ポーションを えた！";

const char msg_treasure_empty_bank[] =
    "からっぽだ。";

const char msg_trap_damage_bank[] =
    "あしもとから\n"
    "どくのけむり！";

const char msg_ruin_lore_bank[] =
    "かべに ふるい\n"
    "もじが きざまれている。";


const char msg_dungeon_npc_guide_bank[] =
    "この どうくつは\n"
    "あしもとに きをつけろ。";

const char msg_dungeon_npc_wounded_bank[] =
    "おくで ひかるものを\n"
    "みたが けむりが ひどい。";

const char msg_ruins_npc_scholar_bank[] =
    "ここは こだいの\n"
    "ちしきが ねむる ばしょ。";

const char msg_ruins_npc_spirit_bank[] =
    "しずかな いしのこえを\n"
    "きけるものだけ すすめ。";

const char msg_heal_spring_bank[] =
    "いずみの ひかりで\n"
    "みんな かいふくした！";

const char msg_pot_found_bank[] =
    "つぼの なかから\n"
    "やくそうを みつけた！";

const char * const message_table[MSG_COUNT] = {
    msg_common_npc_bank,
    msg_test_short_bank,
    msg_growth_hp_bank,
    msg_growth_mp_bank,
    msg_growth_atk_bank,
    msg_growth_def_bank,
    msg_growth_skill_bank,
    msg_growth_heal_bank,
    msg_growth_agi_bank,
    msg_enemy_still_bank,
    msg_enemy_revived_bank,
    msg_leave_as_is_bank,
    msg_enter_town_bank,
    msg_arrive_town_bank,
    msg_back_field_bank,
    msg_left_field_bank,
    msg_later_bank,
    msg_stay_town_bank,
    msg_dark_cave_bank,
    msg_enemy_defeated_revive_bank,
    msg_battle_appear_bank,
    msg_battle_attack_bank,
    msg_battle_low_mp_bank,
    msg_battle_skill_bank,
    msg_battle_hp_full_bank,
    msg_battle_heal_done_bank,
    msg_battle_escape_ok_bank,
    msg_battle_escape_ng_bank,
    msg_battle_enemy_attack_bank,
    msg_battle_win_bank,
    msg_battle_lose_bank,
    msg_prompt_revive_bank,
    msg_prompt_enter_town_bank,
    msg_prompt_exit_town_bank,
    msg_prompt_enter_port_bank,
    msg_enter_port_bank,
    msg_arrive_port_bank,
    msg_prompt_enter_dungeon_bank,
    msg_enter_dungeon_bank,
    msg_arrive_dungeon_bank,
    msg_dungeon_exit_bank,
    msg_prompt_enter_ruins_bank,
    msg_enter_ruins_bank,
    msg_arrive_ruins_bank,
    msg_treasure_found_bank,
    msg_treasure_empty_bank,
    msg_trap_damage_bank,
    msg_ruin_lore_bank,
    msg_dungeon_npc_guide_bank,
    msg_dungeon_npc_wounded_bank,
    msg_ruins_npc_scholar_bank,
    msg_ruins_npc_spirit_bank,
    msg_heal_spring_bank,
    msg_pot_found_bank
};
