#!/usr/bin/env python3
"""
rpg057 pre-bank sanity check.

This is a pre-compile heuristic check only.
Exact bank overflow cannot be determined before compile/link because SDCC/ASlink
layout is decided after object generation.

This script avoids printing fatal linker-warning phrases verbatim, so build
scripts do not false-positive on the precheck log.
"""
from pathlib import Path
import re
import sys

root = Path(__file__).resolve().parents[1]
main = root / "main.c"

def strip_c_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//.*", "", text)
    return text

print("===== prebank_check.py =====")
if not main.exists():
    print("[WARN] main.c not found")
    sys.exit(0)

text = main.read_text(encoding="utf-8", errors="replace")
code = strip_c_comments(text)

print(f"main.c bytes: {main.stat().st_size}")

danger_patterns = [
    "set_win_tiles",
    "DEBUG_OVERLAY",
    "MAP_TILE_SPRING",
    "HEAL_SPRING_TX",
    "try_heal_spring_event",
    "msg_test_npc_short",
    "#include <stdio.h>",
    "#include <gbdk/console.h>",
    "printf(",
    "gotoxy(",
    "cls(",
    "sprintf(",
]


for pat in danger_patterns:
    if pat in code:
        print(f"[WARN] risky/remnant code pattern found in main.c: {pat}")

# Direct graphics calls are risky only if they appear in executable code.
# set_banked_bkg_data()/set_banked_sprite_data() are OK.
direct_bkg = re.search(r"(?<!banked_)set_bkg_data\s*\(", code)
direct_spr = re.search(r"(?<!banked_)set_sprite_data\s*\(", code)
if direct_bkg:
    print("[WARN] direct set_bkg_data() call found in main.c code")
else:
    print("[OK] no direct set_bkg_data() call found in main.c code")

if direct_spr:
    print("[WARN] direct set_sprite_data() call found in main.c code")
else:
    print("[OK] no direct set_sprite_data() call found in main.c code")

static_chars = re.findall(r"static\s+const\s+char\s+\w+\s*\[\]\s*=", code)
if static_chars:
    print("[WARN] static const char[] remains in main.c code:")
    for item in static_chars:
        print("  " + item)
else:
    print("[OK] no static const char[] message definitions found in main.c code")

if "message_show(" not in code:
    print("[WARN] message_show() not used in main.c code")
else:
    print("[OK] message_show() is referenced")


map_array_names = [
    "collision16_map",
    "object16_map",
    "town_collision16_map",
    "town_object16_map",
]
left_in_main = [name for name in map_array_names if re.search(r"const\s+unsigned\s+char\s+" + name + r"\s*\[", code)]
if left_in_main:
    print("[WARN] map data arrays still appear in main.c code: " + ", ".join(left_in_main))
else:
    print("[OK] map data arrays are not defined in main.c code")



jpfont = root / "jpfont.c"
jpfont_h = root / "jpfont.h"
if jpfont.exists():
    jptext = jpfont.read_text(encoding="utf-8", errors="replace")
    if "#pragma bank 5" not in jptext and "BANKREF(jpfont_bank)" not in jptext:
        print("[OK] jpfont.c is non-banked again")
    else:
        print("[WARN] jpfont.c still appears banked")
if jpfont_h.exists():
    jph = jpfont_h.read_text(encoding="utf-8", errors="replace")
    if "jp_put_bkg_text" in jph and "BANKED;" not in jph:
        print("[OK] jpfont.h public APIs are non-banked")
    else:
        print("[WARN] jpfont.h may still expose BANKED jpfont APIs")






font_bank = root / "font_data_bank.c"
misaki_src = root / "misakiUTF16.c"
if font_bank.exists():
    fbtext = font_bank.read_text(encoding="utf-8", errors="replace")
    if "#pragma bank 6" in fbtext and "BANKREF(font_data_bank)" in fbtext:
        print("[OK] font_data_bank.c is bank 6")
    else:
        print("[WARN] font_data_bank.c bank markers missing")
if misaki_src.exists():
    mitext2 = misaki_src.read_text(encoding="utf-8", errors="replace")
    if "misakiUTF16FontData.h" not in mitext2 and "font_data_bank.h" in mitext2:
        print("[OK] misakiUTF16.c uses font data wrappers")
    else:
        print("[WARN] misakiUTF16.c may still include raw font data")


# rpg066 font data split check
font_bank = root / "font_data_bank.c"
font_runtime = root / "font_data_runtime.c"
misaki_src = root / "misakiUTF16.c"

if font_bank.exists():
    fbtext = font_bank.read_text(encoding="utf-8", errors="replace")
    if "#pragma bank 6" in fbtext and "misakiUTF16FontData.h" in fbtext and "BANKREF(font_data_bank)" in fbtext:
        print("[OK] font_data_bank.c owns raw Misaki tables in bank 6")
    else:
        print("[WARN] font_data_bank.c raw-table bank markers may be incomplete")

if font_runtime.exists():
    frtext = font_runtime.read_text(encoding="utf-8", errors="replace")
    if "SWITCH_ROM(BANK(font_data_bank))" in frtext and "NONBANKED" in frtext:
        print("[OK] font_data_runtime.c uses NONBANKED wrappers")
    else:
        print("[WARN] font_data_runtime.c wrapper pattern may be incomplete")

if misaki_src.exists():
    mitext = misaki_src.read_text(encoding="utf-8", errors="replace")
    if "misakiUTF16FontData.h" not in mitext and "font_data_bank.h" in mitext and "return fdata;" not in mitext:
        print("[OK] misakiUTF16.c no longer references raw fdata/ftable directly")
    else:
        print("[WARN] misakiUTF16.c may still reference raw font data")


if main.exists():
    main_text2 = main.read_text(encoding="utf-8", errors="replace")
    if "actor_visible_in_current_area" in main_text2:
        print("[OK] actor visibility predicate is present")
    else:
        print("[WARN] actor visibility predicate missing")


if main.exists():
    main_text3 = main.read_text(encoding="utf-8", errors="replace")
    if "field_random_encounter_should_start" in main_text3:
        print("[OK] random encounter helper is present")
    else:
        print("[WARN] random encounter helper missing")
    if "battle_start_effect" in main_text3:
        print("[OK] battle start effect helper is present")
    else:
        print("[WARN] battle start effect helper missing")


jpfont_src = root / "jpfont.c"
if jpfont_src.exists():
    jpt = jpfont_src.read_text(encoding="utf-8", errors="replace")
    if "charUFT8toUTF16(&utf16" in jpt and "utf16_HantoZen(utf16)" in jpt:
        print("[OK] normalized UTF-16 font cache key is restored")
    else:
        print("[WARN] jpfont cache key may still be byte-based")

print("[INFO] exact bank overflow cannot be known before compile/link.")
print("[INFO] final judgment must use compiler/linker output plus romusage.")
sys.exit(0)
