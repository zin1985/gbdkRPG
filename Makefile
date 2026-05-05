OUT = rpg077_battle.gb
SRCS = main.c sprites.c dialogue.c jpfont.c misakiUTF16.c messages_bank.c messages_runtime.c banked_graphics.c map_data_bank.c map_data_runtime.c font_data_bank.c font_data_runtime.c battle_data_bank.c battle_data_runtime.c
GBDK_PATH ?= /opt/gbdk/bin
LCC = $(GBDK_PATH)/lcc
ROMUSAGE ?= romusage
LOG = build.log
PRECHECK_LOG = precheck_output.log
COMPILE_LOG = compile_output.log
ROMUSAGE_LOG = romusage_output.log

LCC_FLAGS = -msm83:gb -Wl-m -Wl-j -Wl-yt0x19 -Wl-yo8

SHELL := /bin/bash

all:
	@rm -f $(LOG) $(PRECHECK_LOG) $(COMPILE_LOG) $(ROMUSAGE_LOG) dangerous_warnings.log warning_keywords.log romusage_danger.log
	@echo "Building $(OUT) ..." | tee $(LOG)
	@echo "Sources: $(SRCS)" | tee -a $(LOG)
	@if command -v python3 >/dev/null 2>&1 && [ -f tools/prebank_check.py ]; then python3 tools/prebank_check.py > $(PRECHECK_LOG) 2>&1 || true; cat $(PRECHECK_LOG) | tee -a $(LOG); else echo "[WARN] python3 or tools/prebank_check.py not available; pre-bank heuristic check skipped." | tee -a $(LOG); fi
	@echo "" | tee -a $(LOG)
	@echo "Command: $(LCC) $(LCC_FLAGS) -o $(OUT) $(SRCS)" | tee -a $(LOG)
	@set -o pipefail; $(LCC) $(LCC_FLAGS) -o $(OUT) $(SRCS) > $(COMPILE_LOG) 2>&1
	@cat $(COMPILE_LOG) | tee -a $(LOG)
	@echo "" | tee -a $(LOG)
	@echo "[OK] Build finished: $(OUT)" | tee -a $(LOG)
	@echo "" | tee -a $(LOG)
	@echo "===== dangerous linker warning check =====" | tee -a $(LOG)
	@grep -i "Write from one bank spans into the next\|ASlink-Warning\|Possible overflow\|Multiple write\|overflow\|relocation truncated\|segment overlap" $(COMPILE_LOG) > dangerous_warnings.log 2>/dev/null || true
	@if [ -s dangerous_warnings.log ]; then cat dangerous_warnings.log | tee -a $(LOG); echo "[ERROR] Dangerous compiler/linker warning detected. Treating build as failed." | tee -a $(LOG); exit 1; else echo "[OK] no dangerous compiler/linker warning keywords found" | tee -a $(LOG); fi
	@echo "" | tee -a $(LOG)
	@echo "===== romusage check =====" | tee -a $(LOG)
	@if command -v $(ROMUSAGE) >/dev/null 2>&1; then 		echo "[INFO] romusage found: $$(command -v $(ROMUSAGE))" | tee -a $(LOG); 		if [ -f "$(OUT:.gb=.map)" ]; then 			echo "---- romusage $(OUT:.gb=.map) -a -g -B ----" | tee -a $(LOG); 			$(ROMUSAGE) $(OUT:.gb=.map) -a -g -B > $(ROMUSAGE_LOG) 2>&1 || true; 			cat $(ROMUSAGE_LOG) | tee -a $(LOG); 			echo "---- romusage $(OUT:.gb=.map) -q -E ----" | tee -a $(LOG); 			$(ROMUSAGE) $(OUT:.gb=.map) -q -E 2>&1 | tee -a $(LOG) || true; 			grep -i "Possible overflow beyond Bank 0\|Possible overflow\|Multiple write\|Write from one bank spans into the next" $(ROMUSAGE_LOG) > romusage_danger.log 2>/dev/null || true; 			if [ -s romusage_danger.log ]; then 				echo "[ERROR] Dangerous romusage warning detected. Treating build as failed." | tee -a $(LOG); 				cat romusage_danger.log | tee -a $(LOG); 				exit 1; 			fi; 		else 			echo "[WARN] map file not found: $(OUT:.gb=.map)" | tee -a $(LOG); 		fi; 	else 		echo "[WARN] romusage command not found. Install/check GBDK tools if bank usage detail is needed." | tee -a $(LOG); 	fi
	@echo "" | tee -a $(LOG)
	@echo "===== generated files =====" | tee -a $(LOG)
	@ls -l *.gb *.map *.noi *.sym 2>/dev/null | tee -a $(LOG) || true

clean:
	rm -f $(OUT) *.gb *.o *.asm *.ihx *.lst *.map *.noi *.sym *.rel build.log precheck_output.log compile_output.log romusage_output.log dangerous_warnings.log warning_keywords.log romusage_danger.log
