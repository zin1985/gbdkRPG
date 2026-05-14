#!/bin/sh
set -eu
export PATH="${GBDK_PATH:-/opt/gbdk/bin}:$PATH"

if command -v make >/dev/null 2>&1 && [ -f Makefile ]; then
    exec make safe
fi

if [ -x ./build.sh ]; then
    exec ./build.sh
fi

echo "[ERROR] Makefile/build.sh not found. Cannot run safe build."
exit 1
