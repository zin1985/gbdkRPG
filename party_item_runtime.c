#pragma bank 25

#include <gb/gb.h>
#include <gb/hardware.h>
#include "inventory.h"
#include "party_item_runtime.h"

BANKREF(party_item_runtime_bank)

#define PARTY_STATUS_POISON 0x01u
#define PARTY_STATUS_SLEEP  0x02u
#define PARTY_STATUS_STUN   0x04u
#define PARTY_STATUS_TIRED  0x08u
#define PARTY_STATUS_BLIND  0x10u
#define PARTY_STATUS_DOWN   0x80u

#define PARTY_MORALE_MAX  31u
#define PARTY_FOCUS_MAX   31u

#define PARTY_ITEM_EFFECT_NONE 0u
#define PARTY_ITEM_EFFECT_HP   1u
#define PARTY_ITEM_EFFECT_MP   2u
#define PARTY_ITEM_EFFECT_UP   3u
#define PARTY_ITEM_EFFECT_REV  4u

static UINT8 party_item_runtime_random_gain(UINT8 max_gain, UINT8 seed_mix) {
    UINT8 r;
    if (max_gain == 0u) return 0u;
    r = (UINT8)(DIV_REG ^ LY_REG ^ seed_mix);
    return (UINT8)((r % max_gain) + 1u);
}

static UINT8 party_item_runtime_apply_u8_gain(UINT8 *value, UINT8 max_value, UINT8 max_gain, UINT8 *last_amount, UINT8 *last_kind) {
    UINT8 gain;
    if (*value >= max_value) return 0u;
    gain = party_item_runtime_random_gain(max_gain, *last_amount);
    if ((UINT8)(*value + gain) > max_value) gain = (UINT8)(max_value - *value);
    *value = (UINT8)(*value + gain);
    *last_amount = gain;
    *last_kind = PARTY_ITEM_EFFECT_UP;
    return 1u;
}

static UINT8 party_item_runtime_heal_hp(UINT16 *hp, UINT16 max_hp, UINT16 heal, UINT8 *last_amount, UINT8 *last_kind) {
    UINT16 before;
    UINT16 after;
    if (*hp >= max_hp) return 0u;
    before = *hp;
    after = (UINT16)(*hp + heal);
    *hp = (after > max_hp) ? max_hp : after;
    *last_amount = (UINT8)(*hp - before);
    *last_kind = PARTY_ITEM_EFFECT_HP;
    return 1u;
}

static UINT8 party_item_runtime_heal_mp(UINT16 *mp, UINT16 max_mp, UINT16 heal, UINT8 *last_amount, UINT8 *last_kind) {
    UINT16 before;
    UINT16 after;
    if (*mp >= max_mp) return 0u;
    before = *mp;
    after = (UINT16)(*mp + heal);
    *mp = (after > max_mp) ? max_mp : after;
    *last_amount = (UINT8)(*mp - before);
    *last_kind = PARTY_ITEM_EFFECT_MP;
    return 1u;
}

UINT8 party_item_runtime_apply(UINT8 item_id,
                               UINT16 *hp, UINT16 max_hp,
                               UINT16 *mp, UINT16 max_mp,
                               UINT8 *status_flags,
                               UINT8 *defense,
                               UINT8 *attack,
                               UINT8 *agility,
                               UINT8 *focus,
                               UINT8 *morale,
                               UINT8 *last_amount,
                               UINT8 *last_kind) BANKED {
    UINT16 before;

    *last_amount = 0u;
    *last_kind = PARTY_ITEM_EFFECT_NONE;

    switch (item_id) {
        case ITEM_HERB:
            return party_item_runtime_heal_hp(hp, max_hp, (UINT16)(18u + party_item_runtime_random_gain(12u, *last_amount)), last_amount, last_kind);
        case ITEM_MEDICINE:
            return party_item_runtime_heal_hp(hp, max_hp, (UINT16)(28u + party_item_runtime_random_gain(12u, *last_amount)), last_amount, last_kind);
        case ITEM_POTION:
            return party_item_runtime_heal_hp(hp, max_hp, (UINT16)(48u + party_item_runtime_random_gain(20u, *last_amount)), last_amount, last_kind);
        case ITEM_HIGH_POTION:
            return party_item_runtime_heal_hp(hp, max_hp, (UINT16)(75u + party_item_runtime_random_gain(30u, *last_amount)), last_amount, last_kind);
        case ITEM_NAGAI_TEA:
            return party_item_runtime_heal_hp(hp, max_hp, (UINT16)(60u + party_item_runtime_random_gain(20u, *last_amount)), last_amount, last_kind);

        case ITEM_ELIXIR:
        case ITEM_FULL_HERB:
        case ITEM_TENT:
            if (*hp >= max_hp && *mp >= max_mp) return 0u;
            before = *hp;
            *hp = max_hp;
            *last_amount = (UINT8)(*hp - before);
            *last_kind = PARTY_ITEM_EFFECT_HP;
            if (item_id != ITEM_FULL_HERB) {
                before = *mp;
                *mp = max_mp;
                if (*last_amount == 0u) {
                    *last_amount = (UINT8)(*mp - before);
                    *last_kind = PARTY_ITEM_EFFECT_MP;
                }
            }
            return 1u;

        case ITEM_MANA_HERB:
        case ITEM_MANA_DROP:
            return party_item_runtime_heal_mp(mp, max_mp, (UINT16)(8u + party_item_runtime_random_gain(6u, *last_amount)), last_amount, last_kind);
        case ITEM_MANA_BOTTLE:
            return party_item_runtime_heal_mp(mp, max_mp, (UINT16)(20u + party_item_runtime_random_gain(10u, *last_amount)), last_amount, last_kind);

        case ITEM_ANTIDOTE:
        case ITEM_POISON_GUARD:
            if ((*status_flags & PARTY_STATUS_POISON) == 0u) return 0u;
            *status_flags &= (UINT8)~PARTY_STATUS_POISON;
            return 1u;
        case ITEM_WAKE_HERB:
        case ITEM_SLEEP_GUARD:
            if ((*status_flags & PARTY_STATUS_SLEEP) == 0u) return 0u;
            *status_flags &= (UINT8)~PARTY_STATUS_SLEEP;
            return 1u;
        case ITEM_EYE_DROP:
            if ((*status_flags & PARTY_STATUS_BLIND) == 0u) return 0u;
            *status_flags &= (UINT8)~PARTY_STATUS_BLIND;
            return 1u;
        case ITEM_PANACEA:
            if (*status_flags == 0u) return 0u;
            *status_flags = 0u;
            return 1u;

        case ITEM_BARRIER_SEED:
        case ITEM_GUARD_SEED:
            return party_item_runtime_apply_u8_gain(defense, 99u, 3u, last_amount, last_kind);
        case ITEM_POWER_SEED:
            return party_item_runtime_apply_u8_gain(attack, 99u, 3u, last_amount, last_kind);
        case ITEM_SPEED_SEED:
            return party_item_runtime_apply_u8_gain(agility, 99u, 3u, last_amount, last_kind);
        case ITEM_FOCUS_TEA:
            return party_item_runtime_apply_u8_gain(focus, PARTY_FOCUS_MAX, 3u, last_amount, last_kind);
        case ITEM_MORALE_MEAT:
            return party_item_runtime_apply_u8_gain(morale, PARTY_MORALE_MAX, 3u, last_amount, last_kind);

        case ITEM_REVIVE_STONE:
            if (*hp != 0u) return 0u;
            *hp = (UINT16)(max_hp >> 1u);
            if (*hp == 0u) *hp = 1u;
            *last_amount = (UINT8)(*hp > 255u ? 255u : *hp);
            *last_kind = PARTY_ITEM_EFFECT_REV;
            return 1u;

        default:
            return 0u;
    }
}
