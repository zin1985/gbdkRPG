# BUILD_006_NOTE

rpg_005 showed a white screen after changing the build flags to MBC1 + 4 ROM banks.

rpg_006 keeps source files unchanged and only restores build flags to the v3j-confirmed stable values:

```sh
-msm83:gb -Wl-yt0x19 -Wl-yo8
```

If rpg_006 starts normally, the white screen was caused by the build setting change in rpg_005.
If rpg_006 still shows a white screen, next rollback target should be the visual tile changes in sprites.c.
