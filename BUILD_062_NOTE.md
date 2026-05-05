# BUILD 062 NOTE

## Problem in rpg061

Build failed in `misakiUTF16.c`:

```text
misakiUTF16.c:13: warning 278: return type of function omitted, assuming int
misakiUTF16.c:22: syntax error: token -> '{'
```

Cause:

`misakiUTF16.c` used:

```c
BANKREF(misaki_bank)
```

but did not include `gb/gb.h`, where GBDK's BANKREF macro is defined.

Without the macro definition, the compiler parsed BANKREF as an invalid old-style declaration, leading to the later syntax error.

## Fix

Added:

```c
#include <gb/gb.h>
```

near the top of `misakiUTF16.c` before `BANKREF(misaki_bank)`.

## Not changed

- jpfont.c remains in ROM bank 5.
- misakiUTF16.c remains in ROM bank 5.
- jpfont public APIs remain BANKED.
- No new graphics, actors, object kinds, or map data.
