#!/bin/sh
set -eu

export PATH="/opt/gbdk/bin:$PATH"

OUT="rpg078_ui.gb"
MAP="${OUT%.gb}.map"
SRC="main.c sprites.c dialogue.c jpfont.c misakiUTF16.c messages_bank.c messages_runtime.c banked_graphics.c map_data_bank.c map_data_runtime.c font_data_bank.c font_data_runtime.c battle_data_bank.c battle_data_runtime.c"
LOG="build.log"
PRECHECK_LOG="precheck_output.log"
COMPILE_LOG="compile_output.log"
ROMUSAGE_LOG="romusage_output.log"

rm -f "$LOG" "$PRECHECK_LOG" "$COMPILE_LOG" "$ROMUSAGE_LOG" dangerous_warnings.log warning_keywords.log romusage_danger.log

echo "Building ${OUT} ..." | tee "$LOG"
echo "Sources: ${SRC}" | tee -a "$LOG"
echo "" | tee -a "$LOG"

if command -v python3 >/dev/null 2>&1 && [ -f tools/prebank_check.py ]; then
    python3 tools/prebank_check.py > "$PRECHECK_LOG" 2>&1 || true
    cat "$PRECHECK_LOG" | tee -a "$LOG"
    echo "" | tee -a "$LOG"
else
    echo "[WARN] python3 or tools/prebank_check.py not available; pre-bank heuristic check skipped." | tee -a "$LOG"
    echo "" | tee -a "$LOG"
fi

echo "Command: lcc -msm83:gb -Wl-m -Wl-j -Wl-yt0x19 -Wl-yo8 -o ${OUT} ${SRC}" | tee -a "$LOG"
echo "" | tee -a "$LOG"

if lcc -msm83:gb -Wl-m -Wl-j -Wl-yt0x19 -Wl-yo8 -o "$OUT" $SRC > "$COMPILE_LOG" 2>&1; then
    cat "$COMPILE_LOG" | tee -a "$LOG"
    echo "" | tee -a "$LOG"
    echo "[OK] Build finished: ${OUT}" | tee -a "$LOG"
else
    rc=$?
    cat "$COMPILE_LOG" | tee -a "$LOG"
    echo "" | tee -a "$LOG"
    echo "[ERROR] Build failed: exit=${rc}" | tee -a "$LOG"
    exit "$rc"
fi

echo "" | tee -a "$LOG"
echo "===== dangerous linker warning check =====" | tee -a "$LOG"
grep -i "Write from one bank spans into the next\|ASlink-Warning\|Possible overflow\|Multiple write\|overflow\|relocation truncated\|segment overlap" "$COMPILE_LOG" > dangerous_warnings.log 2>/dev/null || true
if [ -s dangerous_warnings.log ]; then
    cat dangerous_warnings.log | tee -a "$LOG"
    echo "[ERROR] Dangerous compiler/linker warning detected. Treating build as failed." | tee -a "$LOG"
    exit 1
else
    echo "[OK] no dangerous compiler/linker warning keywords found" | tee -a "$LOG"
fi

echo "" | tee -a "$LOG"
echo "===== warning keyword check from compiler output =====" | tee -a "$LOG"
grep -i "warning\|error\|overflow\|ASlink\|bank\|relocation\|segment\|area\|spans" "$COMPILE_LOG" > warning_keywords.log 2>/dev/null || true
if [ -s warning_keywords.log ]; then
    cat warning_keywords.log | tee -a "$LOG"
else
    echo "(no matching warning keywords found in compiler output)" | tee -a "$LOG"
fi

echo "" | tee -a "$LOG"
echo "===== romusage check =====" | tee -a "$LOG"
if command -v romusage >/dev/null 2>&1; then
    echo "[INFO] romusage found: $(command -v romusage)" | tee -a "$LOG"
    if [ -f "$MAP" ]; then
        echo "" | tee -a "$LOG"
        echo "---- romusage ${MAP} -a -g -B ----" | tee -a "$LOG"
        romusage "$MAP" -a -g -B > "$ROMUSAGE_LOG" 2>&1 || true
        cat "$ROMUSAGE_LOG" | tee -a "$LOG"
        echo "" | tee -a "$LOG"
        echo "---- romusage ${MAP} -q -E ----" | tee -a "$LOG"
        romusage "$MAP" -q -E 2>&1 | tee -a "$LOG" || true

        grep -i "Possible overflow beyond Bank 0\|Possible overflow\|Multiple write\|Write from one bank spans into the next" "$ROMUSAGE_LOG" > romusage_danger.log 2>/dev/null || true
        if [ -s romusage_danger.log ]; then
            echo "" | tee -a "$LOG"
            echo "[ERROR] Dangerous romusage warning detected. Treating build as failed." | tee -a "$LOG"
            cat romusage_danger.log | tee -a "$LOG"
            exit 1
        fi
    else
        echo "[WARN] map file not found: ${MAP}" | tee -a "$LOG"
    fi
else
    echo "[WARN] romusage command not found. Install/check GBDK tools if bank usage detail is needed." | tee -a "$LOG"
fi

echo "" | tee -a "$LOG"
echo "===== generated files =====" | tee -a "$LOG"
ls -l *.gb *.map *.noi *.sym 2>/dev/null | tee -a "$LOG" || true
