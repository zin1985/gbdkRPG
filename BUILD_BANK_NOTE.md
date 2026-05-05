# v3p-005 build bank note

This version fixes the build error:

```text
error: ROM is too large for number of banks specified
```

by changing the build command to:

```sh
lcc -Wl-yt1 -Wl-yo4 -o rpg.gb main.c sprites.c dialogue.c jpfont.c misakiUTF16.c
```

Meaning:

- `-Wl-yt1`: use MBC1
- `-Wl-yo4`: use 4 ROM banks

This version does **not** add new sprites, new BG tiles, new object kinds, new actors, or new NPCs.
It only changes the cartridge/bank setting used at build time.

If the project later grows again, the next safe step is to increase to `-Wl-yo8` or use `-Wl-yoA`, but do not do that until needed.
