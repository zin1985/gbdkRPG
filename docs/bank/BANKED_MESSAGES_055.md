# BANKED MESSAGES 055

## Why

rpg050 white-screened with:

```text
Warning: Write from one bank spans into the next. 0x7fff -> 0x8001 (bank 1 -> 2)
```

That strongly suggests bank overflow / bank boundary corruption.

## What changed

Message text is no longer stored directly in `main.c`.

Instead:

- `main.c` keeps only small `message_id` values.
- `messages_bank.c` stores the actual message strings in ROM bank 2.
- `messages_runtime.c` switches to the message bank, copies text into WRAM, restores the previous bank, then calls `dialogue_message()`.

## Files

```text
messages.h
messages_bank.c
messages_runtime.c
tools/prebank_check.py
```

## Important

Exact bank overflow cannot be known before compile/link.

`tools/prebank_check.py` is only a heuristic pre-check.  
The final judgment must be based on:

- build.log
- ASlink warnings
- ihxcheck/makebin warnings
- romusage results

The build scripts now fail if they see:

- `Write from one bank spans into the next`
- `ASlink-Warning`
- `overflow`
- `relocation truncated`
- `segment overlap`
