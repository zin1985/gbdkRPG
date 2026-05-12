#!/usr/bin/env python3
"""Strict post-build checker for the GBDK RPG project.

Usage:
  python3 tools/check_build_log_strict.py [build.log] [romusage_output.log]

The normal build can produce a .gb even when the linker emitted bank overlap or
bank overflow warnings. For this project those warnings are a hard failure.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

DANGEROUS_PATTERNS = [
    r"Possible overflow(?: beyond Bank 0| from Bank 0 into Bank 1)?",
    r"Multiple write",
    r"Write from one bank spans into the next",
    r"ASlink-Warning",
    r"relocation truncated",
    r"segment overlap",
    r"Undefined Global",
    r"_[_A-Za-z0-9]+ referenced by module",
]

BANK_OVERFLOW_PATTERNS = [
    r"Bank\s*19.*overflow",
    r"overflow.*Bank\s*19",
    r"Bank\s*0.*overflow",
    r"overflow.*Bank\s*0",
]

WARNING_RE = re.compile("|".join(f"(?:{p})" for p in DANGEROUS_PATTERNS + BANK_OVERFLOW_PATTERNS), re.I)


def read_file(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(errors="replace")


def main() -> int:
    paths = [Path(arg) for arg in sys.argv[1:]] or [Path("build.log"), Path("compile_output.log"), Path("romusage_output.log")]
    hits: list[tuple[str, str]] = []

    for path in paths:
        text = read_file(path)
        if not text:
            continue
        for line in text.splitlines():
            if WARNING_RE.search(line):
                hits.append((str(path), line.strip()))

    if hits:
        print("[ERROR] Dangerous build warning detected. Treat this build as FAILED.")
        for path, line in hits:
            print(f"{path}: {line}")
        return 1

    print("[OK] no dangerous Bank/linker warning detected in checked logs")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
