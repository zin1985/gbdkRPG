# BUILD WARNING CHECK FIX 057

## Problem in rpg056

The actual compile/link output showed no bank-span warnings, but the build failed because the warning checker scanned `build.log`.

`build.log` also included:

- precheck explanatory text
- phrases such as the bank-span warning name
- grep output being appended into the same file being grepped

This caused:

```text
grep: build.log: input file is also the output
```

and a false failure.

## Fix

rpg057 separates logs:

- `precheck_output.log`
- `compile_output.log`
- `build.log`

Fatal linker warning detection only scans `compile_output.log`.

## Also fixed

`prebank_check.py` now strips C comments before pattern checking, so comments like:

```c
Do not call set_bkg_data()
```

do not trigger a false warning.

## Important

If `compile_output.log` contains any real warning such as:

- bank-span warning
- `ASlink-Warning`
- `Possible overflow`
- `Multiple write`
- `relocation truncated`
- `segment overlap`

the build still fails.
