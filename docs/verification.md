# Verification record

Date: 2026-10-06, America/El_Salvador.

**BUILD VERIFIED — HARDWARE NOT VERIFIED.** No Pico, Nitro Mesh, MIDI circuit or PS3 was connected during this work. Stock PS3 recognition remains an acceptance test, not an observed result.

## Build and artifact results

All three variants successfully compiled and linked for RP2040 with the official SDK and toolchain:

| Variant | Purpose | Artifacts |
|---|---|---|
| standard | First-boot P0, cymbals folded to colors | `dist/standard/nitro_ps3.{uf2,elf,bin}` |
| standard-debug | Same USB identity, GP4 UART diagnostics | `dist/standard-debug/nitro_ps3.{uf2,elf,bin}` |
| pro | Experimental separate cymbal classification | `dist/pro/nitro_ps3.{uf2,elf,bin}` |

Environment: Apple Silicon macOS; Arm GNU 14.2.Rel1 / GCC 14.2.1; newlib 4.4.0; native AppleClang 21; CMake 4.4.4; Ninja 1.13.2; Python 3.14.8; Pico SDK 2.2.0 and picotool 2.2.0. Full source revisions are in `dependencies.lock.json`. The Windows PowerShell wrapper is supplied but not executed here.

The standard BIN is 25,472 bytes and its UF2 contains 100 blocks (51,200 bytes). Official picotool recognizes its RP2040 family and flash image starting at `0x10000000`. Standard UF2 SHA-256:

```text
60f7e1b6316d2501c72fcbf210f7baa0933ead9e42f3e562fc2cdf7f6ddcab64
```

A **clean rebuild** of standard firmware produced byte-identical BIN, UF2 **and ELF** on the tested machine/toolchain/path. Build-date metadata is disabled. This is a same-environment reproducibility check; arbitrary compilers/operating systems or changed source/build paths are not asserted to produce identical ELF debugging metadata. Runtime source versions are pinned; use the same complete toolchain for release reproduction.

## Automated tests

`ctest --test-dir build-tests --output-on-failure`: **3/3 passed** (standard core, Pro core, upstream protocol comparison). Core suites also passed with **AddressSanitizer and UndefinedBehaviorSanitizer** in standard and Pro modes.

Covered cases:

- Every configured kick, pad, rim, hi-hat variant and cymbal; ignored pedal/splash and unknown notes.
- Running status, zero-velocity Note On, Note Off, channel decoding, interleaved realtime messages, SysEx/common boundaries and interrupted messages.
- Combined red+yellow+kick and blue+green reports.
- Repeated same-lane strikes, queued velocities, 100Hz simulated snare hits and 200 alternating strikes without queue loss.
- Host backpressure cannot erase an unobserved hit or release; bounded overflow is counted.
- Microsecond timer wrap, hat conversion, navigation bits, bounded feature GET_REPORT responses and Pro yellow/blue tie handling.
- 4,064 standard states (32 lane subsets × 127 velocities) compared byte-for-byte against the actual vendored Santroller packed struct, including offset/size assertions.

Host timing tests simulate transfers; they are not PS3 timing measurements.

## Compiled USB / UF2 audit

Each variant's `verification.json` records successful checks:

1. Full upstream Santroller HID descriptor macro vs selected compiled macro: **byte-identical**, 148 bytes.
2. Native descriptor output vs descriptor symbols read from the **actual ARM ELF**: equal.
3. VID 12BA / PID 0210 / device revision 0200, HID interface, 64-byte interrupt OUT 01 and IN 81 endpoints, 1ms interval.
4. HID parsing: 27-byte input, 8-byte output, declared 32-byte feature report; eight-byte initialization response matches upstream drum case.
5. UF2 magic, RP2040 family ID E48BFF56, block numbering/count, flash addresses and payload sizes valid.
6. Reassembled UF2 payload equals BIN, with only zero alignment padding.
7. Boot2 checksum independently verified.

HID report descriptor SHA-256: `d6688c1a6dcc4ffd83ac63e756157b68e2e9798c94a032b218712c89728c1886`.

## Final static timing/race review

The USB completion callback, main-loop scheduler and MIDI event mapper all execute in TinyUSB's foreground task/main context. The UART interrupt only writes a lock-protected byte queue and increments error counters. Drum phases do not advance while a submitted report is outstanding, so completion acknowledges the correct snapshot. Separate per-lane queues preserve overlapping lanes and repeated attacks. The minimum pulse and release start from acknowledged transfers, not from unobserved wall-clock deadlines. No blocking debug prints, flash writes or deliberate sleeps are used in the play loop.

Review found and corrected a failed-transfer recovery path: the final firmware handles TinyUSB's IN-failure callback, clears the busy flag, and leaves unobserved state pending for retry. Lifecycle reset also clears buffered MIDI. UART error/overflow recovery flushes incomplete data and resets running status; the parser waits for a new channel status rather than inventing notes from a damaged stream. These deliberate recovery drops are reported in diagnostics; a malformed electrical stream cannot be promised lossless.

Remaining material limits: queue capacity is finite, logging can drop text, main-loop host behavior is simulated, game sampling may require hold tuning, GPIO debounce is not physically tested, exact 6N138 edges need measurement, and optional Pro shared-field chord limitations remain. No physical enumeration/game/roll result is inferred from this review. Follow `ps3-test-plan.md`.
