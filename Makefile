override OUT := rpg353_special_battle_recursion_fix.gbc
MAP = $(basename $(OUT)).map
SRCS = main.c audio.c sprites.c dialogue.c jpfont.c jp_backbuffer_runtime.c misakiUTF16.c messages_bank.c messages_runtime.c banked_graphics.c cgb_fx_runtime.c menu_text_dict.c map_data_bank.c map_data_runtime.c font_data_bank.c font_data_runtime.c battle_data_bank.c battle_data_runtime.c battle_text.c game_flags.c quest.c inventory.c ui_icons.c party_runtime.c party_equipment_runtime.c party_item_runtime.c actor_runtime.c menu_runtime.c shop_runtime.c save_runtime.c save_bridge_runtime.c revive_runtime.c itil_tower_runtime.c itil_quiz_bank.c map_event_runtime.c battle_skill_runtime.c battle_skill_meta_runtime.c battle_reward_runtime.c battle_command_ui_runtime.c battle_status_ui_runtime.c battle_growth_runtime.c field_overlay_runtime.c field_feature_runtime.c field_map_render_runtime.c heavy_metal_celtic_battle_bgm.c sunset_ruins_field_bgm.c sunset_strings_adventure_field_bgm.c peaceful_balanced_town_bgm.c deep_eerie_motif_dungeon_bgm.c boss_hope_despair_7part_finale_soft6_sad7_bgm.c equip_runtime.c
OBJS = $(SRCS:.c=.rel)

GBDK_PATH ?= /opt/gbdk/bin
LCC ?= $(GBDK_PATH)/lcc
ROMUSAGE ?= romusage
LOG = build.log
PRECHECK_LOG = precheck_output.log
COMPILE_LOG = compile_output.log
ROMUSAGE_LOG = romusage_output.log

LCC_FLAGS = -msm83:gb
LINK_FLAGS = -Wl-m -Wl-j -Wl-yt0x1B -Wl-yo32 -Wl-ya1 -Wm-yC

SHELL := /bin/bash

.PHONY: all safe fast clean distclean danger romusage precheck FORCE_MAIN_RECOMPILE FORCE_FIELD_RENDER_RECOMPILE FORCE_INVENTORY_RECOMPILE FORCE_MENU_RECOMPILE FORCE_SHOP_RECOMPILE FORCE_PARTY_RECOMPILE FORCE_EQUIP_RECOMPILE FORCE_SPRITES_RECOMPILE FORCE_PARTY_EQUIPMENT_RECOMPILE

all: safe

# TRUE fast build:
# - does not clean .rel cache
# - rebuilds changed .c files only
# - links cached .rel files
# - skips prebank_check and romusage
fast: $(OBJS)
	@rm -f $(LOG) $(COMPILE_LOG) dangerous_warnings.log warning_keywords.log
	@echo "Building FAST $(OUT) ..." | tee $(LOG)
	@echo "Sources: $(SRCS)" | tee -a $(LOG)
	@if [ ! -x "$(LCC)" ]; then echo "[ERROR] lcc not found. Set GBDK_PATH or install GBDK before building." | tee -a $(LOG); exit 127; fi
	@echo "Command: $(LCC) $(LCC_FLAGS) $(LINK_FLAGS) -o $(OUT) $(OBJS)" | tee -a $(LOG)
	@set -o pipefail; $(LCC) $(LCC_FLAGS) $(LINK_FLAGS) -o $(OUT) $(OBJS) > $(COMPILE_LOG) 2>&1; rc=$$?; cat $(COMPILE_LOG) | tee -a $(LOG); if [ $$rc -ne 0 ]; then echo "[ERROR] Link failed: exit=$$rc" | tee -a $(LOG); exit $$rc; fi
	@$(MAKE) danger
	@echo "[OK] FAST BUILD COMPLETE" | tee -a $(LOG)

%.rel: %.c
	@echo "[CC] $<"
	@$(LCC) $(LCC_FLAGS) -c $< -o $@

safe:
	@$(MAKE) clean
	@rm -f $(LOG) $(PRECHECK_LOG) $(COMPILE_LOG) $(ROMUSAGE_LOG) dangerous_warnings.log warning_keywords.log romusage_danger.log
	@echo "Building SAFE $(OUT) ..." | tee $(LOG)
	@echo "Sources: $(SRCS)" | tee -a $(LOG)
	@$(MAKE) precheck
	@echo "" | tee -a $(LOG)
	@echo "Command: $(LCC) $(LCC_FLAGS) $(LINK_FLAGS) -o $(OUT) $(SRCS)" | tee -a $(LOG)
	@if [ ! -x "$(LCC)" ]; then echo "[ERROR] lcc not found. Set GBDK_PATH or install GBDK before building." | tee -a $(LOG); exit 127; fi
	@set -o pipefail; $(LCC) $(LCC_FLAGS) $(LINK_FLAGS) -o $(OUT) $(SRCS) > $(COMPILE_LOG) 2>&1; rc=$$?; cat $(COMPILE_LOG) | tee -a $(LOG); if [ $$rc -ne 0 ]; then echo "[ERROR] Link failed: exit=$$rc" | tee -a $(LOG); exit $$rc; fi
	@$(MAKE) danger
	@$(MAKE) romusage
	@echo "[OK] SAFE BUILD COMPLETE" | tee -a $(LOG)

precheck:
	@if command -v python3 >/dev/null 2>&1 && [ -f tools/prebank_check.py ]; then 		echo "===== prebank_check.py =====" | tee -a $(LOG); 		python3 tools/prebank_check.py > $(PRECHECK_LOG) 2>&1 || true; 		cat $(PRECHECK_LOG) | tee -a $(LOG); 	else 		echo "[WARN] python3 or tools/prebank_check.py not available; pre-bank heuristic check skipped." | tee -a $(LOG); 	fi

danger:
	@echo "===== dangerous compiler/linker warning check =====" | tee -a $(LOG)
	@grep -i "Write from one bank spans into the next\|ASlink-Warning\|Possible overflow\|Multiple write\|overflow\|relocation truncated\|segment overlap\|extends past end\|Overflow by\|Undefined Global" $(COMPILE_LOG) > dangerous_warnings.log 2>/dev/null || true
	@if [ -s dangerous_warnings.log ]; then 		cat dangerous_warnings.log | tee -a $(LOG); 		echo "[ERROR] Dangerous compiler/linker warning detected. Treating build as failed." | tee -a $(LOG); 		exit 1; 	else 		echo "[OK] no dangerous compiler/linker warning keywords found" | tee -a $(LOG); 	fi

romusage:
	@if command -v $(ROMUSAGE) >/dev/null 2>&1 && [ -f "$(MAP)" ]; then 		echo "===== romusage check =====" | tee -a $(LOG); 		$(ROMUSAGE) "$(MAP)" -a -g -B > $(ROMUSAGE_LOG) 2>&1 || true; 		cat $(ROMUSAGE_LOG) | tee -a $(LOG); 		grep -i "Possible overflow\|Multiple write\|Write from one bank spans into the next\|extends past end\|Overflow by" $(ROMUSAGE_LOG) > romusage_danger.log 2>/dev/null || true; 		if [ -s romusage_danger.log ]; then 			cat romusage_danger.log | tee -a $(LOG); 			echo "[ERROR] Dangerous romusage warning detected. Treating build as failed." | tee -a $(LOG); 			exit 1; 		fi; 	else 		echo "[WARN] romusage skipped." | tee -a $(LOG); 	fi

clean:
	rm -f *.asm *.lst *.rel *.sym *.noi *.map *.ihx *.gb *.gbc *.o *.lk *.adb

distclean: clean
	rm -f $(LOG) $(PRECHECK_LOG) $(COMPILE_LOG) $(ROMUSAGE_LOG) dangerous_warnings.log warning_keywords.log romusage_danger.log


# rpg263 GBC extension-safe fix:
# Fast builds may keep an old main.rel from a previous CGB-runtime attempt.
# Force recompilation of main.c so stale references such as _cgb_runtime_init disappear.
main.rel: FORCE_MAIN_RECOMPILE

FORCE_MAIN_RECOMPILE:
	@rm -f main.rel main.asm main.lst main.sym main.o main.adb field_map_render_runtime.rel field_map_render_runtime.asm field_map_render_runtime.lst field_map_render_runtime.sym field_map_render_runtime.o field_map_render_runtime.adb cgb_runtime.rel cgb_runtime.asm cgb_runtime.lst cgb_runtime.sym cgb_runtime.o cgb_runtime.adb

# Force recompilation of field renderer after CGB helper BANKED removal.
.PHONY: FORCE_FIELD_RENDER_RECOMPILE
field_map_render_runtime.rel: FORCE_FIELD_RENDER_RECOMPILE
FORCE_FIELD_RENDER_RECOMPILE:
	@rm -f field_map_render_runtime.rel field_map_render_runtime.asm field_map_render_runtime.lst field_map_render_runtime.sym field_map_render_runtime.o field_map_render_runtime.adb

# Force recompilation after rpg264 inventory speed/CGB UI changes.
inventory.rel: FORCE_INVENTORY_RECOMPILE
FORCE_INVENTORY_RECOMPILE:
	@rm -f inventory.rel inventory.asm inventory.lst inventory.sym inventory.o inventory.adb


# Force recompilation after rpg265 CGB menu/shop/status/inventory backbuffer changes.
menu_runtime.rel: FORCE_MENU_RECOMPILE
FORCE_MENU_RECOMPILE:
	@rm -f menu_runtime.rel menu_runtime.asm menu_runtime.lst menu_runtime.sym menu_runtime.o menu_runtime.adb

shop_runtime.rel: FORCE_SHOP_RECOMPILE
FORCE_SHOP_RECOMPILE:
	@rm -f shop_runtime.rel shop_runtime.asm shop_runtime.lst shop_runtime.sym shop_runtime.o shop_runtime.adb

party_runtime.rel: FORCE_PARTY_RECOMPILE
FORCE_PARTY_RECOMPILE:
	@rm -f party_runtime.rel party_runtime.asm party_runtime.lst party_runtime.sym party_runtime.o party_runtime.adb

# Force recompilation after rpg266 CGB FX/UI stability changes.
cgb_fx_runtime.rel: FORCE_CGB_FX_RECOMPILE
FORCE_CGB_FX_RECOMPILE:
	@rm -f cgb_fx_runtime.rel cgb_fx_runtime.asm cgb_fx_runtime.lst cgb_fx_runtime.sym cgb_fx_runtime.o cgb_fx_runtime.adb

# Force recompilation after rpg266 save/title/battle-status CGB fixes.
save_runtime.rel: FORCE_SAVE_RECOMPILE
FORCE_SAVE_RECOMPILE:
	@rm -f save_runtime.rel save_runtime.asm save_runtime.lst save_runtime.sym save_runtime.o save_runtime.adb

battle_status_ui_runtime.rel: FORCE_BATTLE_STATUS_RECOMPILE
FORCE_BATTLE_STATUS_RECOMPILE:
	@rm -f battle_status_ui_runtime.rel battle_status_ui_runtime.asm battle_status_ui_runtime.lst battle_status_ui_runtime.sym battle_status_ui_runtime.o battle_status_ui_runtime.adb

# Force recompilation after rpg267 item page slide animation changes.
inventory.rel: FORCE_INVENTORY_SLIDE_RECOMPILE
FORCE_INVENTORY_SLIDE_RECOMPILE:
	@rm -f inventory.rel inventory.asm inventory.lst inventory.sym inventory.o inventory.adb

# Force recompilation after rpg268 item/menu slide and equipment freeze fixes.
equip_runtime.rel: FORCE_EQUIP_RECOMPILE
FORCE_EQUIP_RECOMPILE:
	@rm -f equip_runtime.rel equip_runtime.asm equip_runtime.lst equip_runtime.sym equip_runtime.o equip_runtime.adb


# Force recompilation after dictionary/token message changes.
menu_text_dict.rel: FORCE_MENU_TEXT_DICT_RECOMPILE
FORCE_MENU_TEXT_DICT_RECOMPILE:
	@rm -f menu_text_dict.rel menu_text_dict.asm menu_text_dict.lst menu_text_dict.sym menu_text_dict.o menu_text_dict.adb inventory.rel party_runtime.rel equip_runtime.rel shop_runtime.rel battle_reward_runtime.rel




# Force recompilation after party equipment table bank split.
party_equipment_runtime.rel: FORCE_PARTY_EQUIPMENT_RECOMPILE
FORCE_PARTY_EQUIPMENT_RECOMPILE:
	@rm -f party_equipment_runtime.rel party_equipment_runtime.asm party_equipment_runtime.lst party_equipment_runtime.sym party_equipment_runtime.o party_equipment_runtime.adb

# Force recompilation after warmachine M/L sprite replacement.
sprites.rel: FORCE_SPRITES_RECOMPILE
FORCE_SPRITES_RECOMPILE:
	@rm -f sprites.rel sprites.asm sprites.lst sprites.sym sprites.o sprites.adb


# Force recompilation after party field-item helper bank split.
party_item_runtime.rel: FORCE_PARTY_ITEM_RECOMPILE
FORCE_PARTY_ITEM_RECOMPILE:
	@rm -f party_item_runtime.rel party_item_runtime.asm party_item_runtime.lst party_item_runtime.sym party_item_runtime.o party_item_runtime.adb
