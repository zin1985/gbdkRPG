#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Build helper for menu/message dictionary token streams.

This script is intentionally source-side only.  Human-readable text may use
named tokens such as {ITEM_HERB}, {TERM_HEAL}, {TERM_EFFECT}.  The script can
later be expanded to emit compact C token arrays for dialogue/message banks.

Current project policy:
- Do not hand-write numeric [1], [2] tokens in scenario text.
- Write named tokens, then generate numeric token streams at build time.
- Keep generated data in banked ROM, never in Bank 0.
"""
from __future__ import annotations
import json
import re
import sys
from pathlib import Path

ESC = 0x1F

DEFAULT_TERMS = {
    "TERM_HEAL": "かいふく",
    "TERM_EFFECT": "効果",
    "TERM_ITEM": "どうぐ",
    "ITEM_HERB": "やくそう",
    "ITEM_POTION": "ポーション",
}

TOKEN_RE = re.compile(r"\{([A-Z0-9_]+)\}")

def encode_named_tokens(text: str, terms: dict[str, str]) -> list[int]:
    out: list[int] = []
    pos = 0
    token_ids = {name: i + 1 for i, name in enumerate(sorted(terms))}
    for m in TOKEN_RE.finditer(text):
        out.extend(text[pos:m.start()].encode("utf-8"))
        name = m.group(1)
        if name not in token_ids:
            raise SystemExit(f"unknown token: {name}")
        out.extend([ESC, token_ids[name]])
        pos = m.end()
    out.extend(text[pos:].encode("utf-8"))
    out.append(0)
    return out

def main() -> None:
    if len(sys.argv) < 2:
        print("usage: build_message_dict.py 'message with {ITEM_HERB}' [terms.json]")
        return
    terms = DEFAULT_TERMS
    if len(sys.argv) >= 3:
        terms = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
    encoded = encode_named_tokens(sys.argv[1], terms)
    print(", ".join(f"0x{x:02X}" for x in encoded))

if __name__ == "__main__":
    main()
