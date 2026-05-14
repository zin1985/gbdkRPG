#pragma bank 25

#include <gb/gb.h>
#include "menu_text_dict.h"
#include "inventory.h"
#include "jpfont.h"

BANKREF(menu_text_dict_bank)

#define MENU_DICT_ESC 0x1Fu
#define MENU_DICT_BUF_LEN 32u
#define MENU_DICT_BUF_COUNT 4u
static char g_menu_dict_text_buf[MENU_DICT_BUF_COUNT][MENU_DICT_BUF_LEN];
static UINT8 g_menu_dict_buf_index;

#define MENU_DICT_WORD_NO 1u
#define MENU_DICT_WORD_KEN 2u
#define MENU_DICT_WORD_TSUE 3u
#define MENU_DICT_WORD_YUMI 4u
#define MENU_DICT_WORD_YOROI 5u
#define MENU_DICT_WORD_POTION 6u
#define MENU_DICT_WORD_MAHOU 7u
#define MENU_DICT_WORD_KUSA 8u
#define MENU_DICT_WORD_BIN 9u
#define MENU_DICT_WORD_MIZU 10u
#define MENU_DICT_WORD_TANE 11u
#define MENU_DICT_WORD_BOMB 12u
#define MENU_DICT_WORD_RING 13u
#define MENU_DICT_WORD_TEKOU 14u
#define MENU_DICT_WORD_DOUGU 15u
#define MENU_DICT_WORD_KIT 16u
#define MENU_DICT_WORD_KUSURI 17u
#define MENU_DICT_WORD_MAMORI 18u
#define MENU_DICT_WORD_YUBIWA 19u
#define MENU_DICT_WORD_MEDAL 20u
#define MENU_DICT_WORD_ITIL 21u
#define MENU_DICT_WORD_KAIFUKU 22u
#define MENU_DICT_WORD_KOUKA 23u
#define MENU_DICT_WORD_ITEMS 24u
#define MENU_DICT_WORD_A_USE_B_BACK 25u
#define MENU_DICT_WORD_A_USE_B_RETURN_HIRA 26u
#define MENU_DICT_WORD_NO_ITEM 27u
#define MENU_DICT_WORD_NO_EFFECT 28u
#define MENU_DICT_WORD_TARGET 29u
#define MENU_DICT_WORD_CANNOT_USE 30u
#define MENU_DICT_WORD_USED 31u
#define MENU_DICT_WORD_UPPED 32u
#define MENU_DICT_WORD_EMPTY_USABLE 33u
#define MENU_DICT_WORD_SAME 34u
#define MENU_DICT_WORD_ITEM_KIND 35u

static const char md_word_NO[] = "の";
static const char md_word_KEN[] = "けん";
static const char md_word_TSUE[] = "つえ";
static const char md_word_YUMI[] = "ゆみ";
static const char md_word_YOROI[] = "よろい";
static const char md_word_POTION[] = "ポーション";
static const char md_word_MAHOU[] = "まほう";
static const char md_word_KUSA[] = "草";
static const char md_word_BIN[] = "びん";
static const char md_word_MIZU[] = "水";
static const char md_word_TANE[] = "種";
static const char md_word_BOMB[] = "ばくだん";
static const char md_word_RING[] = "リング";
static const char md_word_TEKOU[] = "手甲";
static const char md_word_DOUGU[] = "道具";
static const char md_word_KIT[] = "キット";
static const char md_word_KUSURI[] = "薬";
static const char md_word_MAMORI[] = "守り";
static const char md_word_YUBIWA[] = "指輪";
static const char md_word_MEDAL[] = "メダル";
static const char md_word_ITIL[] = "アイティル";
static const char md_word_KAIFUKU[] = "かいふく";
static const char md_word_KOUKA[] = "こうか";
static const char md_word_ITEMS[] = "もちもの";
static const char md_word_A_USE_B_BACK[] = "A=使う B=戻る";
static const char md_word_A_USE_B_RETURN_HIRA[] = "A=つかう B=もどる";
static const char md_word_NO_ITEM[] = "なにもない";
static const char md_word_NO_EFFECT[] = "こうかが ない";
static const char md_word_TARGET[] = "だれに つかう?";
static const char md_word_CANNOT_USE[] = "そのままでは つかえない";
static const char md_word_USED[] = "つかった!";
static const char md_word_UPPED[] = "あがった";
static const char md_word_EMPTY_USABLE[] = "使える道具なし";
static const char md_word_SAME[] = "同じ";
static const char md_word_ITEM_KIND[] = "どうぐ";

static const char *menu_dict_word(UINT8 word_id) {
    switch (word_id) {
        case MENU_DICT_WORD_NO: return md_word_NO;
        case MENU_DICT_WORD_KEN: return md_word_KEN;
        case MENU_DICT_WORD_TSUE: return md_word_TSUE;
        case MENU_DICT_WORD_YUMI: return md_word_YUMI;
        case MENU_DICT_WORD_YOROI: return md_word_YOROI;
        case MENU_DICT_WORD_POTION: return md_word_POTION;
        case MENU_DICT_WORD_MAHOU: return md_word_MAHOU;
        case MENU_DICT_WORD_KUSA: return md_word_KUSA;
        case MENU_DICT_WORD_BIN: return md_word_BIN;
        case MENU_DICT_WORD_MIZU: return md_word_MIZU;
        case MENU_DICT_WORD_TANE: return md_word_TANE;
        case MENU_DICT_WORD_BOMB: return md_word_BOMB;
        case MENU_DICT_WORD_RING: return md_word_RING;
        case MENU_DICT_WORD_TEKOU: return md_word_TEKOU;
        case MENU_DICT_WORD_DOUGU: return md_word_DOUGU;
        case MENU_DICT_WORD_KIT: return md_word_KIT;
        case MENU_DICT_WORD_KUSURI: return md_word_KUSURI;
        case MENU_DICT_WORD_MAMORI: return md_word_MAMORI;
        case MENU_DICT_WORD_YUBIWA: return md_word_YUBIWA;
        case MENU_DICT_WORD_MEDAL: return md_word_MEDAL;
        case MENU_DICT_WORD_ITIL: return md_word_ITIL;
        case MENU_DICT_WORD_KAIFUKU: return md_word_KAIFUKU;
        case MENU_DICT_WORD_KOUKA: return md_word_KOUKA;
        case MENU_DICT_WORD_ITEMS: return md_word_ITEMS;
        case MENU_DICT_WORD_A_USE_B_BACK: return md_word_A_USE_B_BACK;
        case MENU_DICT_WORD_A_USE_B_RETURN_HIRA: return md_word_A_USE_B_RETURN_HIRA;
        case MENU_DICT_WORD_NO_ITEM: return md_word_NO_ITEM;
        case MENU_DICT_WORD_NO_EFFECT: return md_word_NO_EFFECT;
        case MENU_DICT_WORD_TARGET: return md_word_TARGET;
        case MENU_DICT_WORD_CANNOT_USE: return md_word_CANNOT_USE;
        case MENU_DICT_WORD_USED: return md_word_USED;
        case MENU_DICT_WORD_UPPED: return md_word_UPPED;
        case MENU_DICT_WORD_EMPTY_USABLE: return md_word_EMPTY_USABLE;
        case MENU_DICT_WORD_SAME: return md_word_SAME;
        case MENU_DICT_WORD_ITEM_KIND: return md_word_ITEM_KIND;
        default: return "";
    }
}

static const char md_item_ITEM_HERB[] = "やくそう";
static const char md_item_ITEM_POTION[] = "\x1F" "\x06";
static const char md_item_ITEM_CASTLE_KEY[] = "しろ" "\x1F" "\x01" "かぎ";
static const char md_item_ITEM_MANA_HERB[] = "\x1F" "\x07" "\x1F" "\x08";
static const char md_item_ITEM_ANTIDOTE[] = "どくけし";
static const char md_item_ITEM_WOOD_SWORD[] = "木" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_IRON_SWORD[] = "鉄" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_OAK_STAFF[] = "木" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_MAGE_STAFF[] = "まどう" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_SHORT_BOW[] = "短い" "\x1F" "\x04";
static const char md_item_ITEM_GLOVES[] = "けいこ" "\x1F" "\x0E";
static const char md_item_ITEM_TOOL_KIT[] = "\x1F" "\x0F" "\x1F" "\x10";
static const char md_item_ITEM_CLOTH_ARMOR[] = "ぬ" "\x1F" "\x01" "\x1F" "\x01" "ふく";
static const char md_item_ITEM_LEATHER_ARMOR[] = "かわ" "\x1F" "\x05";
static const char md_item_ITEM_IRON_ARMOR[] = "鉄" "\x1F" "\x05";
static const char md_item_ITEM_CHARM[] = "おまもり";
static const char md_item_ITEM_FEATHER[] = "はね飾り";
static const char md_item_ITEM_DEBUG_NO_ENCOUNT[] = "退魔" "\x1F" "\x01" "すず";
static const char md_item_ITEM_DEBUG_ESCAPE[] = "はやあし" "\x1F" "\x0D";
static const char md_item_ITEM_MEDICINE[] = "きずぐすり";
static const char md_item_ITEM_HIGH_POTION[] = "上" "\x1F" "\x06";
static const char md_item_ITEM_NAGAI_TEA[] = "長井茶";
static const char md_item_ITEM_ELIXIR[] = "エリクサ";
static const char md_item_ITEM_MANA_DROP[] = "\x1F" "\x07" "\x1F" "\x0A";
static const char md_item_ITEM_MANA_BOTTLE[] = "\x1F" "\x07" "\x1F" "\x09";
static const char md_item_ITEM_FULL_HERB[] = "全快" "\x1F" "\x08";
static const char md_item_ITEM_PANACEA[] = "ばん" "\x1F" "\x01" "う" "\x1F" "\x11";
static const char md_item_ITEM_WAKE_HERB[] = "めざめ" "\x1F" "\x08";
static const char md_item_ITEM_EYE_DROP[] = "めぐすり";
static const char md_item_ITEM_SMOKE_BOMB[] = "けむり玉";
static const char md_item_ITEM_BARRIER_SEED[] = "まもり" "\x1F" "\x0B";
static const char md_item_ITEM_POWER_SEED[] = "ちから" "\x1F" "\x0B";
static const char md_item_ITEM_GUARD_SEED[] = "かたい" "\x1F" "\x0B";
static const char md_item_ITEM_SPEED_SEED[] = "はやさ" "\x1F" "\x0B";
static const char md_item_ITEM_FOCUS_TEA[] = "集中茶";
static const char md_item_ITEM_MORALE_MEAT[] = "戦意肉";
static const char md_item_ITEM_ICE_BOMB[] = "氷" "\x1F" "\x0C";
static const char md_item_ITEM_FIRE_BOMB[] = "火" "\x1F" "\x0C";
static const char md_item_ITEM_THUNDER_BOMB[] = "雷" "\x1F" "\x0C";
static const char md_item_ITEM_LIGHT_BOTTLE[] = "光" "\x1F" "\x01" "\x1F" "\x09";
static const char md_item_ITEM_DARK_BOTTLE[] = "闇" "\x1F" "\x01" "\x1F" "\x09";
static const char md_item_ITEM_RETURN_FEATHER[] = "帰還羽";
static const char md_item_ITEM_TENT[] = "テント";
static const char md_item_ITEM_REVIVE_STONE[] = "命" "\x1F" "\x01" "石";
static const char md_item_ITEM_COPPER_SWORD[] = "銅" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_STEEL_SWORD[] = "鋼" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_SILVER_SWORD[] = "銀" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_FLAME_SWORD[] = "炎" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_ICE_SWORD[] = "氷" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_DRAGON_SWORD[] = "竜" "\x1F" "\x01" "\x1F" "\x02";
static const char md_item_ITEM_WILLOW_STAFF[] = "柳" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_SAGE_STAFF[] = "賢者" "\x1F" "\x03";
static const char md_item_ITEM_THUNDER_STAFF[] = "雷" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_MOON_STAFF[] = "月" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_SHADOW_STAFF[] = "影" "\x1F" "\x01" "\x1F" "\x03";
static const char md_item_ITEM_LONG_BOW[] = "長い" "\x1F" "\x04";
static const char md_item_ITEM_STEEL_BOW[] = "鋼" "\x1F" "\x01" "\x1F" "\x04";
static const char md_item_ITEM_HUNTER_BOW[] = "狩人" "\x1F" "\x04";
static const char md_item_ITEM_THUNDER_BOW[] = "雷" "\x1F" "\x01" "\x1F" "\x04";
static const char md_item_ITEM_STAR_BOW[] = "星" "\x1F" "\x01" "\x1F" "\x04";
static const char md_item_ITEM_BRONZE_SPEAR[] = "青銅" "\x1F" "\x01" "やり";
static const char md_item_ITEM_STEEL_SPEAR[] = "鋼" "\x1F" "\x01" "やり";
static const char md_item_ITEM_DRAGON_SPEAR[] = "竜槍";
static const char md_item_ITEM_HAND_AXE[] = "手お" "\x1F" "\x01";
static const char md_item_ITEM_BATTLE_AXE[] = "戦お" "\x1F" "\x01";
static const char md_item_ITEM_GREAT_AXE[] = "大お" "\x1F" "\x01";
static const char md_item_ITEM_IRON_GLOVES[] = "鉄" "\x1F" "\x0E";
static const char md_item_ITEM_BEAST_CLAWS[] = "獣" "\x1F" "\x01" "爪";
static const char md_item_ITEM_POWER_KNUCKLE[] = "力" "\x1F" "\x0E";
static const char md_item_ITEM_MONK_FIST[] = "僧" "\x1F" "\x01" "拳";
static const char md_item_ITEM_ALCHEMY_KIT[] = "錬金具";
static const char md_item_ITEM_TRAP_TOOL[] = "罠" "\x1F" "\x0F";
static const char md_item_ITEM_BOMB_TOOL[] = "爆弾具";
static const char md_item_ITEM_GEAR_TOOL[] = "歯車具";
static const char md_item_ITEM_TRAVEL_CLOTH[] = "旅" "\x1F" "\x01" "服";
static const char md_item_ITEM_MAGE_ROBE[] = "魔法ローブ";
static const char md_item_ITEM_BATTLE_CLOTH[] = "戦い服";
static const char md_item_ITEM_CHAIN_MAIL[] = "鎖かたびら";
static const char md_item_ITEM_STEEL_ARMOR[] = "鋼" "\x1F" "\x05";
static const char md_item_ITEM_SILVER_ARMOR[] = "銀" "\x1F" "\x05";
static const char md_item_ITEM_DRAGON_ARMOR[] = "竜" "\x1F" "\x05";
static const char md_item_ITEM_QUICK_BOOTS[] = "早足くつ";
static const char md_item_ITEM_POWER_RING[] = "力" "\x1F" "\x01" "\x1F" "\x13";
static const char md_item_ITEM_GUARD_RING[] = "\x1F" "\x12" "\x1F" "\x13";
static const char md_item_ITEM_FOCUS_RING[] = "集中" "\x1F" "\x13";
static const char md_item_ITEM_LUCKY_COIN[] = "幸運コイン";
static const char md_item_ITEM_DRAGON_SCALE[] = "竜" "\x1F" "\x01" "うろこ";
static const char md_item_ITEM_SLEEP_GUARD[] = "眠り" "\x1F" "\x12";
static const char md_item_ITEM_POISON_GUARD[] = "毒" "\x1F" "\x12";
static const char md_item_ITEM_LIGHT_MEDAL[] = "光" "\x1F" "\x14";
static const char md_item_ITEM_DARK_MEDAL[] = "闇" "\x1F" "\x14";
static const char md_item_ITEM_ITIL_SWORD[] = "\x1F" "\x15" "\x1F" "\x01" "剣";
static const char md_item_ITEM_ITIL_ARMOR[] = "\x1F" "\x15" "\x1F" "\x01" "鎧";
static const char md_item_ITEM_ITIL_CREST[] = "\x1F" "\x15" "\x1F" "\x01" "紋章";
static const char md_item_unknown[] = "?";

static const char md_short_ITEM_HIGH_POTION[] = "上ポー";
static const char md_short_ITEM_MANA_BOTTLE[] = "魔" "\x1F" "\x09";
static const char md_short_ITEM_DEBUG_NO_ENCOUNT[] = "退魔鈴";
static const char md_short_ITEM_DEBUG_ESCAPE[] = "早足輪";
static const char md_short_ITEM_RETURN_FEATHER[] = "帰還羽";
static const char md_short_ITEM_REVIVE_STONE[] = "命" "\x1F" "\x01" "石";
static const char md_short_ITEM_BRONZE_SPEAR[] = "青銅槍";
static const char md_short_ITEM_STEEL_SPEAR[] = "鋼槍";
static const char md_short_ITEM_HAND_AXE[] = "手お" "\x1F" "\x01";
static const char md_short_ITEM_BATTLE_AXE[] = "戦お" "\x1F" "\x01";
static const char md_short_ITEM_POWER_KNUCKLE[] = "力" "\x1F" "\x0E";
static const char md_short_ITEM_DRAGON_SCALE[] = "竜うろ";

static const char md_msg_ITEM_HELP[] = "\x1F" "\x1A";
static const char md_msg_ITEM_HELP_KANJI[] = "\x1F" "\x19";
static const char md_msg_EMPTY_USABLE[] = "\x1F" "\x21";
static const char md_msg_TARGET[] = "\x1F" "\x1D";
static const char md_msg_NO_EFFECT[] = "\x1F" "\x1C";
static const char md_msg_ITEMS_TITLE[] = "\x1F" "\x18";
static const char md_msg_EMPTY[] = "\x1F" "\x1B";
static const char md_msg_CANNOT_USE[] = "\x1F" "\x1E";
static const char md_msg_USED[] = "\x1F" "\x1F";
static const char md_msg_UPPED[] = "\x1F" "\x20";
static const char md_msg_HEAL[] = "\x1F" "\x16";
static const char md_msg_TARGET_NO_SPACE[] = "だれにつかう？";
static const char md_msg_SHOP_ITEM[] = "\x1F" "\x0F" "や";
static const char md_msg_SHOP_WEAPON[] = "ぶぐや";
static const char md_msg_SHOP_HELP[] = "A かう  B やめる";
static const char md_msg_SHOP_BOUGHT[] = "かった！";
static const char md_msg_NO_MONEY[] = "Gが たりない";
static const char md_msg_INN_TITLE[] = "やどや";
static const char md_msg_INN_ASK[] = "10Gで とまりますか";
static const char md_msg_INN_HELP[] = "A とまる  B やめる";
static const char md_msg_INN_HEALED[] = "みんな かいふくした";
static const char md_msg_BATTLE_ATTACK[] = "こうげき";
static const char md_msg_BATTLE_MAGIC[] = "\x1F" "\x07";
static const char md_msg_BATTLE_SKILL[] = "とくぎ";
static const char md_msg_BATTLE_DEFENSE[] = "ぼうぎょ";
static const char md_msg_BATTLE_ESCAPE[] = "にげる";
static const char md_msg_MENU_TITLE[] = "メニュー";
static const char md_msg_MENU_STATUS[] = "つよさ";
static const char md_msg_MENU_EQUIP[] = "そうび";
static const char md_msg_MENU_OBJECTIVE[] = "もくてき";
static const char md_msg_ITEM_KIND[] = "\x1F" "\x23";

static const char *menu_dict_item_encoded(UINT8 item_id) {
    switch (item_id) {
        case ITEM_HERB: return md_item_ITEM_HERB;
        case ITEM_POTION: return md_item_ITEM_POTION;
        case ITEM_CASTLE_KEY: return md_item_ITEM_CASTLE_KEY;
        case ITEM_MANA_HERB: return md_item_ITEM_MANA_HERB;
        case ITEM_ANTIDOTE: return md_item_ITEM_ANTIDOTE;
        case ITEM_WOOD_SWORD: return md_item_ITEM_WOOD_SWORD;
        case ITEM_IRON_SWORD: return md_item_ITEM_IRON_SWORD;
        case ITEM_OAK_STAFF: return md_item_ITEM_OAK_STAFF;
        case ITEM_MAGE_STAFF: return md_item_ITEM_MAGE_STAFF;
        case ITEM_SHORT_BOW: return md_item_ITEM_SHORT_BOW;
        case ITEM_GLOVES: return md_item_ITEM_GLOVES;
        case ITEM_TOOL_KIT: return md_item_ITEM_TOOL_KIT;
        case ITEM_CLOTH_ARMOR: return md_item_ITEM_CLOTH_ARMOR;
        case ITEM_LEATHER_ARMOR: return md_item_ITEM_LEATHER_ARMOR;
        case ITEM_IRON_ARMOR: return md_item_ITEM_IRON_ARMOR;
        case ITEM_CHARM: return md_item_ITEM_CHARM;
        case ITEM_FEATHER: return md_item_ITEM_FEATHER;
        case ITEM_DEBUG_NO_ENCOUNT: return md_item_ITEM_DEBUG_NO_ENCOUNT;
        case ITEM_DEBUG_ESCAPE: return md_item_ITEM_DEBUG_ESCAPE;
        case ITEM_MEDICINE: return md_item_ITEM_MEDICINE;
        case ITEM_HIGH_POTION: return md_item_ITEM_HIGH_POTION;
        case ITEM_NAGAI_TEA: return md_item_ITEM_NAGAI_TEA;
        case ITEM_ELIXIR: return md_item_ITEM_ELIXIR;
        case ITEM_MANA_DROP: return md_item_ITEM_MANA_DROP;
        case ITEM_MANA_BOTTLE: return md_item_ITEM_MANA_BOTTLE;
        case ITEM_FULL_HERB: return md_item_ITEM_FULL_HERB;
        case ITEM_PANACEA: return md_item_ITEM_PANACEA;
        case ITEM_WAKE_HERB: return md_item_ITEM_WAKE_HERB;
        case ITEM_EYE_DROP: return md_item_ITEM_EYE_DROP;
        case ITEM_SMOKE_BOMB: return md_item_ITEM_SMOKE_BOMB;
        case ITEM_BARRIER_SEED: return md_item_ITEM_BARRIER_SEED;
        case ITEM_POWER_SEED: return md_item_ITEM_POWER_SEED;
        case ITEM_GUARD_SEED: return md_item_ITEM_GUARD_SEED;
        case ITEM_SPEED_SEED: return md_item_ITEM_SPEED_SEED;
        case ITEM_FOCUS_TEA: return md_item_ITEM_FOCUS_TEA;
        case ITEM_MORALE_MEAT: return md_item_ITEM_MORALE_MEAT;
        case ITEM_ICE_BOMB: return md_item_ITEM_ICE_BOMB;
        case ITEM_FIRE_BOMB: return md_item_ITEM_FIRE_BOMB;
        case ITEM_THUNDER_BOMB: return md_item_ITEM_THUNDER_BOMB;
        case ITEM_LIGHT_BOTTLE: return md_item_ITEM_LIGHT_BOTTLE;
        case ITEM_DARK_BOTTLE: return md_item_ITEM_DARK_BOTTLE;
        case ITEM_RETURN_FEATHER: return md_item_ITEM_RETURN_FEATHER;
        case ITEM_TENT: return md_item_ITEM_TENT;
        case ITEM_REVIVE_STONE: return md_item_ITEM_REVIVE_STONE;
        case ITEM_COPPER_SWORD: return md_item_ITEM_COPPER_SWORD;
        case ITEM_STEEL_SWORD: return md_item_ITEM_STEEL_SWORD;
        case ITEM_SILVER_SWORD: return md_item_ITEM_SILVER_SWORD;
        case ITEM_FLAME_SWORD: return md_item_ITEM_FLAME_SWORD;
        case ITEM_ICE_SWORD: return md_item_ITEM_ICE_SWORD;
        case ITEM_DRAGON_SWORD: return md_item_ITEM_DRAGON_SWORD;
        case ITEM_WILLOW_STAFF: return md_item_ITEM_WILLOW_STAFF;
        case ITEM_SAGE_STAFF: return md_item_ITEM_SAGE_STAFF;
        case ITEM_THUNDER_STAFF: return md_item_ITEM_THUNDER_STAFF;
        case ITEM_MOON_STAFF: return md_item_ITEM_MOON_STAFF;
        case ITEM_SHADOW_STAFF: return md_item_ITEM_SHADOW_STAFF;
        case ITEM_LONG_BOW: return md_item_ITEM_LONG_BOW;
        case ITEM_STEEL_BOW: return md_item_ITEM_STEEL_BOW;
        case ITEM_HUNTER_BOW: return md_item_ITEM_HUNTER_BOW;
        case ITEM_THUNDER_BOW: return md_item_ITEM_THUNDER_BOW;
        case ITEM_STAR_BOW: return md_item_ITEM_STAR_BOW;
        case ITEM_BRONZE_SPEAR: return md_item_ITEM_BRONZE_SPEAR;
        case ITEM_STEEL_SPEAR: return md_item_ITEM_STEEL_SPEAR;
        case ITEM_DRAGON_SPEAR: return md_item_ITEM_DRAGON_SPEAR;
        case ITEM_HAND_AXE: return md_item_ITEM_HAND_AXE;
        case ITEM_BATTLE_AXE: return md_item_ITEM_BATTLE_AXE;
        case ITEM_GREAT_AXE: return md_item_ITEM_GREAT_AXE;
        case ITEM_IRON_GLOVES: return md_item_ITEM_IRON_GLOVES;
        case ITEM_BEAST_CLAWS: return md_item_ITEM_BEAST_CLAWS;
        case ITEM_POWER_KNUCKLE: return md_item_ITEM_POWER_KNUCKLE;
        case ITEM_MONK_FIST: return md_item_ITEM_MONK_FIST;
        case ITEM_ALCHEMY_KIT: return md_item_ITEM_ALCHEMY_KIT;
        case ITEM_TRAP_TOOL: return md_item_ITEM_TRAP_TOOL;
        case ITEM_BOMB_TOOL: return md_item_ITEM_BOMB_TOOL;
        case ITEM_GEAR_TOOL: return md_item_ITEM_GEAR_TOOL;
        case ITEM_TRAVEL_CLOTH: return md_item_ITEM_TRAVEL_CLOTH;
        case ITEM_MAGE_ROBE: return md_item_ITEM_MAGE_ROBE;
        case ITEM_BATTLE_CLOTH: return md_item_ITEM_BATTLE_CLOTH;
        case ITEM_CHAIN_MAIL: return md_item_ITEM_CHAIN_MAIL;
        case ITEM_STEEL_ARMOR: return md_item_ITEM_STEEL_ARMOR;
        case ITEM_SILVER_ARMOR: return md_item_ITEM_SILVER_ARMOR;
        case ITEM_DRAGON_ARMOR: return md_item_ITEM_DRAGON_ARMOR;
        case ITEM_QUICK_BOOTS: return md_item_ITEM_QUICK_BOOTS;
        case ITEM_POWER_RING: return md_item_ITEM_POWER_RING;
        case ITEM_GUARD_RING: return md_item_ITEM_GUARD_RING;
        case ITEM_FOCUS_RING: return md_item_ITEM_FOCUS_RING;
        case ITEM_LUCKY_COIN: return md_item_ITEM_LUCKY_COIN;
        case ITEM_DRAGON_SCALE: return md_item_ITEM_DRAGON_SCALE;
        case ITEM_SLEEP_GUARD: return md_item_ITEM_SLEEP_GUARD;
        case ITEM_POISON_GUARD: return md_item_ITEM_POISON_GUARD;
        case ITEM_LIGHT_MEDAL: return md_item_ITEM_LIGHT_MEDAL;
        case ITEM_DARK_MEDAL: return md_item_ITEM_DARK_MEDAL;
        case ITEM_ITIL_SWORD: return md_item_ITEM_ITIL_SWORD;
        case ITEM_ITIL_ARMOR: return md_item_ITEM_ITIL_ARMOR;
        case ITEM_ITIL_CREST: return md_item_ITEM_ITIL_CREST;
        default: return md_item_unknown;
    }
}

static const char *menu_dict_item_short_encoded(UINT8 item_id) {
    switch (item_id) {
        case ITEM_HIGH_POTION: return md_short_ITEM_HIGH_POTION;
        case ITEM_MANA_BOTTLE: return md_short_ITEM_MANA_BOTTLE;
        case ITEM_DEBUG_NO_ENCOUNT: return md_short_ITEM_DEBUG_NO_ENCOUNT;
        case ITEM_DEBUG_ESCAPE: return md_short_ITEM_DEBUG_ESCAPE;
        case ITEM_RETURN_FEATHER: return md_short_ITEM_RETURN_FEATHER;
        case ITEM_REVIVE_STONE: return md_short_ITEM_REVIVE_STONE;
        case ITEM_BRONZE_SPEAR: return md_short_ITEM_BRONZE_SPEAR;
        case ITEM_STEEL_SPEAR: return md_short_ITEM_STEEL_SPEAR;
        case ITEM_HAND_AXE: return md_short_ITEM_HAND_AXE;
        case ITEM_BATTLE_AXE: return md_short_ITEM_BATTLE_AXE;
        case ITEM_POWER_KNUCKLE: return md_short_ITEM_POWER_KNUCKLE;
        case ITEM_DRAGON_SCALE: return md_short_ITEM_DRAGON_SCALE;
        default: return menu_dict_item_encoded(item_id);
    }
}

static const char *menu_dict_message_encoded(UINT8 msg_id) {
    switch (msg_id) {
        case MENU_DICT_MSG_ITEM_HELP: return md_msg_ITEM_HELP;
        case MENU_DICT_MSG_ITEM_HELP_KANJI: return md_msg_ITEM_HELP_KANJI;
        case MENU_DICT_MSG_EMPTY_USABLE: return md_msg_EMPTY_USABLE;
        case MENU_DICT_MSG_TARGET: return md_msg_TARGET;
        case MENU_DICT_MSG_NO_EFFECT: return md_msg_NO_EFFECT;
        case MENU_DICT_MSG_ITEMS_TITLE: return md_msg_ITEMS_TITLE;
        case MENU_DICT_MSG_EMPTY: return md_msg_EMPTY;
        case MENU_DICT_MSG_CANNOT_USE: return md_msg_CANNOT_USE;
        case MENU_DICT_MSG_USED: return md_msg_USED;
        case MENU_DICT_MSG_UPPED: return md_msg_UPPED;
        case MENU_DICT_MSG_HEAL: return md_msg_HEAL;
        case MENU_DICT_MSG_TARGET_NO_SPACE: return md_msg_TARGET_NO_SPACE;
        case MENU_DICT_MSG_SHOP_ITEM: return md_msg_SHOP_ITEM;
        case MENU_DICT_MSG_SHOP_WEAPON: return md_msg_SHOP_WEAPON;
        case MENU_DICT_MSG_SHOP_HELP: return md_msg_SHOP_HELP;
        case MENU_DICT_MSG_SHOP_BOUGHT: return md_msg_SHOP_BOUGHT;
        case MENU_DICT_MSG_NO_MONEY: return md_msg_NO_MONEY;
        case MENU_DICT_MSG_INN_TITLE: return md_msg_INN_TITLE;
        case MENU_DICT_MSG_INN_ASK: return md_msg_INN_ASK;
        case MENU_DICT_MSG_INN_HELP: return md_msg_INN_HELP;
        case MENU_DICT_MSG_INN_HEALED: return md_msg_INN_HEALED;
        case MENU_DICT_MSG_BATTLE_ATTACK: return md_msg_BATTLE_ATTACK;
        case MENU_DICT_MSG_BATTLE_MAGIC: return md_msg_BATTLE_MAGIC;
        case MENU_DICT_MSG_BATTLE_SKILL: return md_msg_BATTLE_SKILL;
        case MENU_DICT_MSG_BATTLE_DEFENSE: return md_msg_BATTLE_DEFENSE;
        case MENU_DICT_MSG_BATTLE_ESCAPE: return md_msg_BATTLE_ESCAPE;
        case MENU_DICT_MSG_MENU_TITLE: return md_msg_MENU_TITLE;
        case MENU_DICT_MSG_MENU_STATUS: return md_msg_MENU_STATUS;
        case MENU_DICT_MSG_MENU_EQUIP: return md_msg_MENU_EQUIP;
        case MENU_DICT_MSG_MENU_OBJECTIVE: return md_msg_MENU_OBJECTIVE;
        case MENU_DICT_MSG_ITEM_KIND: return md_msg_ITEM_KIND;
        default: return md_msg_ITEM_HELP;
    }
}

static const char *menu_dict_decode(const char *encoded) {
    UINT8 bi;
    UINT8 b;
    UINT8 dst_index;
    char *dst;
    const char *word;
    bi = 0u;
    dst_index = g_menu_dict_buf_index;
    g_menu_dict_buf_index = (UINT8)((g_menu_dict_buf_index + 1u) & 3u);
    dst = g_menu_dict_text_buf[dst_index];
    if (encoded == 0) { dst[0] = 0; return dst; }
    while (*encoded != 0 && bi < (UINT8)(MENU_DICT_BUF_LEN - 1u)) {
        b = (UINT8)*encoded++;
        if (b == MENU_DICT_ESC) {
            word = menu_dict_word((UINT8)*encoded++);
            while (*word != 0 && bi < (UINT8)(MENU_DICT_BUF_LEN - 1u)) {
                dst[bi++] = *word++;
            }
        } else {
            dst[bi++] = (char)b;
        }
    }
    dst[bi] = 0;
    return dst;
}

const char *menu_dict_item_name(UINT8 item_id) BANKED {
    return menu_dict_decode(menu_dict_item_encoded(item_id));
}

const char *menu_dict_item_short_name(UINT8 item_id) BANKED {
    return menu_dict_decode(menu_dict_item_short_encoded(item_id));
}

const char *menu_dict_message(UINT8 msg_id) BANKED {
    return menu_dict_decode(menu_dict_message_encoded(msg_id));
}

void menu_dict_put_item_name(UINT8 x, UINT8 y, UINT8 width, UINT8 item_id) BANKED {
    jp_bkg_clear_area(x, y, width, 1u);
    jp_put_bkg_text(x, y, menu_dict_item_name(item_id));
}

void menu_dict_put_item_short_name(UINT8 x, UINT8 y, UINT8 width, UINT8 item_id) BANKED {
    jp_bkg_clear_area(x, y, width, 1u);
    jp_put_bkg_text(x, y, menu_dict_item_short_name(item_id));
}

void menu_dict_put_message(UINT8 x, UINT8 y, UINT8 width, UINT8 msg_id) BANKED {
    jp_bkg_clear_area(x, y, width, 1u);
    jp_put_bkg_text(x, y, menu_dict_message(msg_id));
}
