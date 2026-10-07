# PS3 protocol derivation

Reference: Santroller commit `272bae8d28e17e9886e73344adeb8788a062eef4`. Relevant unmodified files are retained in `third_party/santroller/`. The compiled header is a transitive macro subset of `hid_reports.h`; the other files are reference material, not additional linked firmware.

| Property | Implemented value | Upstream basis |
|---|---|---|
| Device identity | VID 0x12BA, PID 0x0210, bcdDevice 0x0200 | `PS3GamepadDevice::device_descriptor`, RockBandDrums case |
| Class | Per-interface HID, no boot subclass/protocol | `config_descriptor` third-party branch |
| HID report descriptor | 148 bytes, no report ID | `TUD_HID_REPORT_DESC_PS3_THIRDPARTY_GAMEPAD()` |
| Endpoints | OUT 0x01, IN 0x81; interrupt; 64 bytes; bInterval=1 | Same IN/OUT descriptor macro, buffer size and interval; fixed addresses for this single interface |
| Input | 27 bytes | `PS3RockBandDrums_Data_t`, sent as `PS3Dpad_Data_t` size upstream |
| GET_REPORT feature ID 0 | `21 26 01 05 00 00 00 00` | `ps3_init`, overridden byte 3 for RockBandDrums |
| HID output / feature declaration | 8 / 32 bytes | Upstream descriptor; actual initialization feature reply is 8 bytes as upstream |
| Stick centers / unused sensor fields | 0x80 / little-endian 0x0200 | `initialize()` |
| Mapping | Rock Band color, pedal, pad/cymbal flags | `rock_band_mappings.cpp` PS3 functions |

The firmware's outer device descriptor uses USB 2.0, EP0=64, one configuration, bDeviceClass=0; configuration is bus-powered, requests 100mA and does not advertise remote wake. Project-specific strings replace Santroller branding. Santroller's larger composite/detection framework, WinUSB compatible-section support, configurator commands, Bluetooth, other instruments and DS3-specific features are omitted. Therefore **the report descriptor is byte-identical; the whole multi-device Santroller USB implementation is not claimed identical**. This firmware fixes PS3 drum mode, avoiding auto-detection branches.

## Input transport boundary

The firmware receives UART/DIN MIDI only. A USB-only module requires the external host described in [USB-only setup](usb-midi-setup.md). Its DIN output feeds the unchanged receiver/parser; no USB-MIDI host driver is included in this firmware. The PS3 device descriptor remains unchanged, and the additional host's compatibility/latency require hardware testing.

## Input bytes

| Offset | Meaning |
|---|---|
| 0 | bit0 blue/Square, bit1 green/Cross, bit2 red/Circle, bit3 yellow/Triangle, bit4 kick/L1; bit5 second kick reserved |
| 1 | bit0 Select, bit1 Start, bit2 pad flag/L3, bit3 cymbal flag/R3; PS/Home bit4 unused |
| 2 | Converted HID hat: 0 up, 1 up-right, 2 right, 3 down-right, 4 down, 5 down-left, 6 left, 7 up-left, 8 neutral |
| 3–6 | Four centered axes, 0x80 |
| 7–10 | Zero |
| 11 | Yellow velocity |
| 12 | Red velocity / shared second cymbal velocity slot |
| 13 | Green velocity |
| 14 | Blue velocity |
| 15–18 | Zero, including unused trigger fields |
| 19–26 | Four neutral little-endian 16-bit values 0x0200 |

For an active MIDI velocity `v`, encoded velocity is `255 - floor(v * 255 / 127)`. Zero velocity is not a strike. Inactive velocity bytes are zero, matching upstream initialization; thus the corresponding button/flags are essential to distinguish a maximum-velocity hit from idle. Kick is digital; there is no kick-velocity field. Musical/game response to intermediate values requires testing.

Standard mode folds cymbal events into the corresponding pad lane **before queueing**, so consecutive tom/hat hits of the same color still receive distinct release edges. Hat bits are converted to the HID hat value; an idle bitmask of zero must become **8**, not north. No byte-level bitfield casts are used in firmware; explicit byte encoding is compared against upstream packed structs in native tests.

## Optional Pro mode

`--pro` preserves separate pad and cymbal lanes. It applies the current upstream rules: pad flag for pad hits, cymbal flag for cymbals; yellow cymbal asserts up, blue asserts down, green uses neither. When yellow and blue cymbals coincide, the stronger velocity wins the hat discriminator (yellow wins a tie), as in Santroller. Physical D-pad is overridden by up/down cymbal classification when active. Same-color pad+cymbal uses the red velocity slot when red is absent.

**Known limits:** simultaneous red plus a same-color pad+cymbal pair cannot be expressed by the adapted upstream branches; that pair is omitted for the overlapping state. Multiple same-color pad+cymbal pairs compete for the shared red slot (later color wins). These optional combinations are not claimed supported. Standard mode has no such shared cymbal encoding. Use standard mode for P0; Pro needs RB3-specific testing and potential further work for edge-case chords.

## Control and timing behavior

TinyUSB handles standard enumeration, HID idle/protocol requests and interrupt endpoints. This adapter bounds feature replies to the requested buffer length, accepts/discards LED/output reports, and stalls unsupported feature IDs. GET_REPORT INPUT returns the last prepared state but does not acknowledge a strike; only completed interrupt IN transfers advance hold/release observation.

The state machine is:

```text
queued strike -> pressed, unobserved -> completed IN -> hold >=4ms
              -> released, unobserved -> completed IN -> gap >=1ms
              -> next queued strike or idle
```

USB state is frozen between submission and completion. Incoming MIDI only appends queues. A failed IN callback permits retry without acknowledging the new state. Disconnect, bus-reset/unready, suspend and remount clear the session/queued MIDI. Unsigned timer subtraction handles the ~71-minute microsecond counter wrap. Only the UART byte queue crosses interrupt/main context; SDK queue locks protect that boundary. The firmware uses neither a second core nor flash writes during gameplay.

## Audits

`tests/test_protocol.c` compares all 32 standard lane subsets at every nonzero velocity (4064 states) to the actual upstream packed struct. `scripts/verify.py` compiles the full upstream descriptor macro and the extracted macro separately, compares their bytes, checks the **actual ARM ELF descriptor symbols**, decodes HID report bit counts and validates endpoint/identity values. It also reconstructs the UF2, compares it to BIN and validates the RP2040 boot2 checksum. Outputs are in each `dist/*/verification.json`.

These are host-side/compiled-artifact checks. They cannot observe PS3 enumeration, host polling, game-frame sampling, the physical optocoupler or actual pad behavior.
