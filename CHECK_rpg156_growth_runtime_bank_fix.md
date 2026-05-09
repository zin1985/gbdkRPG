# rpg156_growth_runtime_bank_fix

Bank 0 still overflowed in rpg155 because the battle/growth expansion left too much non-banked code in main.c.

This build moves the battle growth result logic out of main.c and into `battle_growth_runtime.c` in bank 5:

- growth clear
- growth chance calculation
- stat growth application
- growth result message dispatch

The player stat globals and `random_u8()` are exposed so the banked growth module can update the same runtime state without duplicating data.
