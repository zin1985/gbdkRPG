#!/bin/sh
set -eu
export PATH="${GBDK_PATH:-/opt/gbdk/bin}:$PATH"

if command -v make >/dev/null 2>&1 && [ -f Makefile ]; then
    exec make fast
fi

echo "[ERROR] Makefile or make command not found. Cannot run fast build."
exit 1
