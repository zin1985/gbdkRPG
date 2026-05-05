# Documentation Index

This folder was organized in rpg070 to keep the project root focused on source
files and build entry points.

## Folders

- `bank/`  
  Bank split, ROM usage, and memory-layout notes.

- `build/`  
  Build notes and build-system related documents.

- `build_logs/`  
  Generated build logs if they were present in the package.

- `changelog/`  
  Historical changelog.

- `checks/`  
  Static checks, runtime checklists, and verification notes.

- `features/`  
  Feature-specific notes such as area actor visibility, random encounter, and
  font cache cleanup.

- `graphics_safety/`  
  Graphics safety rules, BG/OBJ policy, and white-screen prevention notes.

- `program_flow/`  
  Program flow and debug-analysis notes.

- `notes/`  
  Miscellaneous project notes.

## Root files intentionally kept

- `README.md`
- `build.sh`
- `build.bat`
- `Makefile`
- source files (`*.c`, `*.h`)
- diff files (`*.diff`)

`README.md` remains in the root because GitHub displays it as the repository
landing page.

## rpg071 additions

- `features/BATTLE_ENCOUNTER_TABLE_071.md`
- `checks/CHECK_071_BATTLE_ENCOUNTER_TABLE.md`
- `bank/BANK_071_BATTLE_DATA.md`
