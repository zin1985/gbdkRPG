# BUILD 066 NOTE

## Problem in rpg065

Build failed:

```text
misakiUTF16.c:181: error 20: Undefined identifier 'fdata'
```

Cause:

rpg065 moved the raw Misaki font tables out of `misakiUTF16.c` and into `font_data_bank.c`.

However, this compatibility function still returned the raw table pointer:

```c
const uint8_t*getFontTableAddress(void) {
    return fdata;
}
```

`fdata` no longer exists in `misakiUTF16.c`, so the compile failed.

## Fix

`getFontTableAddress()` now returns NULL:

```c
return (const uint8_t*)0;
```

It is retained only for compatibility and should not be used by gameplay code.

## Important

The real font data access path is now:

```text
misakiUTF16.c
  -> font_data_table_at()
  -> font_data_read()
  -> font_data_runtime.c
  -> font_data_bank.c bank 6
```
