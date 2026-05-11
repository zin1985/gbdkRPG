#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
prebank_static_check.py

GBDK RPG 用の「コンパイル前」Bank配置チェック補助スクリプト。

重要:
- コンパイル/リンク前に、最終的な各Bank使用量や重なりを完全確定することはできない。
- ただし、#pragma bank の重複・危険Bank使用・既知の空き容量傾向・前回mapとの差分を元に、
  「危なそうな配置」をビルド前にかなり早く検出できる。
- 最終判定は必ず lcc 後の linker warning と romusage で行う。

使い方:
    python3 tools/prebank_static_check.py
    python3 tools/prebank_static_check.py --strict
    python3 tools/prebank_static_check.py --map rpg198_bank8_overlap_actor_bank1_fix.map

戻り値:
    0 = 致命的な事前NGなし
    1 = strict時の危険検出、または明確な設定不整合
"""

from __future__ import annotations

import argparse
import os
import re
import sys
from pathlib import Path
from collections import defaultdict

ROM_BANK_SIZE = 0x4000

# このプロジェクトで過去に危険化しやすかったBank。
# Bank 8 は 0x20000 起点の Multiple write が複数回出ているため、
# 原則として新規配置を避ける。
DANGEROUS_BANKS = {
    8: "Bank 8 は過去に 0x20000 起点の Multiple write / Possible overflow が出たため新規配置非推奨",
}

# Bank 0 は main.c / nonbanked / 初期化が厳しい。
BANK0_WARN_THRESHOLD = 0.94
BANK_WARN_THRESHOLD = 0.90

# ソースサイズによる雑な事前警告。
# Cソースbytesはコンパイル後CODEサイズと一致しないが、
# 直近の膨張傾向を見る警告として使う。
SOURCE_BANK_SOFT_LIMIT = 52000
SOURCE_BANK_HARD_LIMIT = 68000

C_FILE_RE = re.compile(r"^([^#].*\.c)\s*(?:\\\\)?$", re.M)
PRAGMA_BANK_RE = re.compile(r"^\s*#\s*pragma\s+bank\s+(\d+)", re.M)
BANK_ID_RE = re.compile(r"#define\s+(ROM_BANK_[A-Z0-9_]+)\s+(\d+)u")

def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")

def parse_build_sources(root: Path) -> list[str]:
    build = root / "build.sh"
    if not build.exists():
        return sorted(p.name for p in root.glob("*.c"))
    txt = read_text(build)
    # lcc commandから .c を拾う。複数行継続にも対応。
    return list(dict.fromkeys(re.findall(r"([A-Za-z0-9_]+\.c)", txt)))

def parse_yo_banks(root: Path) -> int | None:
    build = root / "build.sh"
    if not build.exists():
        return None
    txt = read_text(build)
    m = re.search(r"-Wl-yo(\d+)", txt)
    if m:
        return int(m.group(1))
    return None

def parse_bank_ids(root: Path) -> dict[str, int]:
    path = root / "bank_ids.h"
    if not path.exists():
        return {}
    return {name: int(num) for name, num in BANK_ID_RE.findall(read_text(path))}

def parse_source_banks(root: Path, sources: list[str]) -> dict[str, int]:
    result = {}
    for src in sources:
        path = root / src
        if not path.exists():
            continue
        txt = read_text(path)
        m = PRAGMA_BANK_RE.search(txt)
        if m:
            result[src] = int(m.group(1))
        else:
            result[src] = 0
    return result

def parse_map_usage(map_path: Path) -> dict[int, int]:
    """romusage相当ではなく、.map内の _CODE_N 行からざっくりサイズを読む。"""
    if not map_path or not map_path.exists():
        return {}
    txt = read_text(map_path)
    usage = {}
    # 例: + _CODE_7 0x1C000 -> 0x1FFFF 16384
    for m in re.finditer(r"\+\s+_CODE(?:_(\d+))?\s+0x[0-9A-Fa-f]+\s+->\s+0x[0-9A-Fa-f]+\s+(\d+)", txt):
        bank = int(m.group(1) or 0)
        size = int(m.group(2))
        usage[bank] = usage.get(bank, 0) + size
    return usage

def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".", help="project root")
    ap.add_argument("--strict", action="store_true", help="warnings for dangerous banks fail the check")
    ap.add_argument("--map", default=None, help="optional previous/current .map file for post-link-style usage check")
    args = ap.parse_args()

    root = Path(args.root).resolve()
    sources = parse_build_sources(root)
    source_banks = parse_source_banks(root, sources)
    bank_ids = parse_bank_ids(root)
    yo_banks = parse_yo_banks(root)

    errors = []
    warnings = []

    print("===== prebank_static_check.py =====")
    print(f"root: {root}")
    if yo_banks is not None:
        print(f"ROM banks from build.sh -Wl-yo: {yo_banks}")
    else:
        warnings.append("build.sh の -Wl-yo が見つかりません。ROMバンク数を確認してください。")

    print("\n--- source bank layout (#pragma bank) ---")
    by_bank = defaultdict(list)
    src_size_by_bank = defaultdict(int)
    for src in sources:
        bank = source_banks.get(src, 0)
        size = (root / src).stat().st_size if (root / src).exists() else 0
        by_bank[bank].append((src, size))
        src_size_by_bank[bank] += size

    for bank in sorted(by_bank):
        items = by_bank[bank]
        total = src_size_by_bank[bank]
        label = "ROM_0/nonbanked" if bank == 0 else f"Bank {bank}"
        print(f"{label}: source_bytes={total}")
        for src, size in items:
            print(f"  - {src}: {size}")

    print("\n--- bank_ids.h aliases ---")
    if bank_ids:
        for name, bank in sorted(bank_ids.items(), key=lambda x: (x[1], x[0])):
            print(f"{name} = {bank}")
            if yo_banks is not None and bank >= yo_banks:
                errors.append(f"{name}={bank} は -Wl-yo{yo_banks} の範囲外です。")
    else:
        warnings.append("bank_ids.h が見つからない、またはROM_BANK_*定義が読めません。")

    print("\n--- static risk checks ---")
    if 0 in src_size_by_bank and src_size_by_bank[0] > SOURCE_BANK_HARD_LIMIT:
        warnings.append(f"Bank0/nonbanked source bytes が大きいです: {src_size_by_bank[0]} bytes")
    for bank, total in sorted(src_size_by_bank.items()):
        if bank != 0 and total > SOURCE_BANK_HARD_LIMIT:
            warnings.append(f"Bank {bank} のCソース合計が大きいです: {total} bytes。分割候補です。")
        elif bank != 0 and total > SOURCE_BANK_SOFT_LIMIT:
            warnings.append(f"Bank {bank} のCソース合計がやや大きいです: {total} bytes。romusage確認推奨。")

    for bank in sorted(by_bank):
        if bank in DANGEROUS_BANKS:
            msg = f"Bank {bank}: {DANGEROUS_BANKS[bank]}。配置ファイル: {[s for s,_ in by_bank[bank]]}"
            if args.strict:
                errors.append(msg)
            else:
                warnings.append(msg)

    # 直近の実害に合わせた特別ルール。
    for src, bank in source_banks.items():
        if src == "actor_runtime.c" and bank == 8:
            errors.append("actor_runtime.c が Bank 8 にあります。過去にBank8 Multiple write原因になったため Bank1等へ移動してください。")
        if src == "inventory.c" and bank == 8:
            errors.append("inventory.c が Bank 8 にあります。過去にBank8 Multiple write原因になったため Bank11等へ移動してください。")

    # optional map check
    map_usage = {}
    if args.map:
        map_path = (root / args.map).resolve() if not os.path.isabs(args.map) else Path(args.map)
        map_usage = parse_map_usage(map_path)
        print("\n--- map usage check ---")
        if not map_usage:
            warnings.append(f"map usage が読めません: {map_path}")
        for bank, used in sorted(map_usage.items()):
            pct = used / ROM_BANK_SIZE
            free = ROM_BANK_SIZE - used
            print(f"Bank {bank}: used={used} free={free} used%={pct:.1%}")
            if used > ROM_BANK_SIZE:
                errors.append(f"Bank {bank} overflow: {used - ROM_BANK_SIZE} bytes")
            elif bank == 0 and pct >= BANK0_WARN_THRESHOLD:
                warnings.append(f"Bank 0 残量が少ないです: free={free} bytes")
            elif bank != 0 and pct >= BANK_WARN_THRESHOLD:
                warnings.append(f"Bank {bank} 残量が少ないです: free={free} bytes")

    if warnings:
        print("\n[WARNINGS]")
        for w in warnings:
            print(f"- {w}")

    if errors:
        print("\n[ERRORS]")
        for e in errors:
            print(f"- {e}")
        print("\n[NG] prebank static check failed.")
        return 1

    print("\n[OK] no static bank layout errors found.")
    print("NOTE: final judgment still requires lcc linker output and romusage.")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
