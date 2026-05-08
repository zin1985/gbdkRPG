#!/usr/bin/env python3
from pathlib import Path
import re

def strip_c_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//.*", "", text)
    return text

root = Path(__file__).resolve().parents[1]
print("===== bank_architecture_check.py =====")

def read(name):
    p = root / name
    return p.read_text(encoding="utf-8", errors="replace") if p.exists() else ""

build = read("build.sh")
make = read("Makefile")
field = read("field_feature_runtime.c")
ids = read("bank_ids.h")

checks = [
    ("build.sh output name", "rpg136_scheduled_bank_architecture_pack.gb" in build),
    ("Makefile output name", "rpg136_scheduled_bank_architecture_pack.gb" in make),
    ("build.sh uses 16 ROM banks", "-Wl-yo16" in build),
    ("Makefile uses 16 ROM banks", "-Wl-yo16" in make),
    ("field features moved to bank 5", "#pragma bank 5" in field),
    ("bank_ids.h exists with future save bank", "ROM_BANK_FUTURE_SAVE_COMPAT" in ids),
]

for name, ok in checks:
    print(("[OK] " if ok else "[WARN] ") + name)

main = strip_c_comments(read("main.c"))
if re.search(r"static\s+const\s+char\s+\w+\s*\[\]\s*=", main):
    print("[WARN] main.c still has static const char[] text")
else:
    print("[OK] main.c has no static const char[] text")

if "set_bkg_data(" in main.replace("set_banked_bkg_data(", ""):
    print("[WARN] main.c may contain direct set_bkg_data")
else:
    print("[OK] main.c direct set_bkg_data not detected")
