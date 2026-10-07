# Nitro Pico PS3 Drums

Open-source Alesis Nitro Mesh **5-pin MIDI OUT → Raspberry Pi Pico RP2040 → stock PS3 Rock Band drums** adapter. GPL-3.0-only. No PS3XPAD, HEN, CFW, authentication donor, PC at runtime, or proprietary firmware is required by this design.

**BUILD VERIFIED. HARDWARE NOT VERIFIED.** The standard, debug, and experimental Pro builds are intended for physical testing. Compilation and protocol comparison are not proof that a particular PS3/game revision will accept this adapter. See [verification](docs/verification.md) and the [first-boot test plan](docs/ps3-test-plan.md).

## Flash first

Use **[dist/standard/nitro_ps3.uf2](dist/standard/nitro_ps3.uf2)** for initial testing. ELF, BIN, checksums and descriptor audit files are beside it. Hold the Pico's BOOTSEL button while connecting USB to your computer, release when `RPI-RP2` appears, and copy the UF2 onto that drive. The drive disconnects as the Pico reboots. Then disconnect from the computer and connect the Pico's USB port to the PS3. Repeat BOOTSEL to restore/change firmware; no programmer is needed. This image targets the original RP2040 Pico, not Pico 2.

## Architecture and hardware

```text
Nitro Mesh MIDI OUT -- DIN cable -- isolated MIDI IN -- GP1 / UART0 RX
                                                      Pico RP2040
                                                           |
                                                    USB device port
                                                           |
                                                   Stock PS3 + Rock Band
```

Use an original Raspberry Pi Pico, USB **data** cable, 5-pin 180-degree female DIN socket, 6N138 optocoupler, 220Ω/1kΩ/4.7kΩ resistors, 1N4148 protection diode, 100nF capacitor, breadboard or soldered board, and optionally six normally-open momentary switches. Nitro retains its own power supply. Pico is powered from PS3 USB. A 3.3V USB-UART adapter is useful for diagnostics but is not needed for gameplay.

**Build the complete circuit in [docs/wiring.md](docs/wiring.md). Never connect DIN MIDI directly to GP1.** That document includes a pin-by-pin netlist, an ASCII schematic, connector-view precautions, electrical references and checks.

| Function | Pico GPIO | Physical header pin |
|---|---|---|
| MIDI input, 31250 baud 8-N-1 | GP1 / UART0 RX | 2 |
| Optional diagnostic output, 115200 baud 8-N-1 | GP4 / UART1 TX | 6 |
| Start / Select | GP6 / GP7 | 9 / 10 |
| D-pad Up / Down | GP8 / GP9 | 11 / 12 |
| D-pad Left / Right | GP10 / GP11 | 14 / 15 |
| Circuit GND | GND | 3 (other Pico GND pins also work) |
| RX pull-up supply | 3V3 OUT | 36 |
| 6N138 supply | USB VBUS, nominal 5V | 40 |

Buttons connect their GPIO to Pico GND when pressed. Firmware enables pull-ups and applies 5ms debounce. Leave them disconnected if unused. Keep a normal PS3 controller available for XMB navigation and game launch; this adapter has no PS/Home button. USB uses the Pico's built-in connector and does not consume a header GPIO.

## MIDI mapping

Defaults come from the [Nitro Drum Module User Guide v1.2, appendix page 38](https://www.alesis.com/rscdn/1886/documents/Nitro%20Drum%20Module%20-%20User%20Guide%20-%20v1.2.pdf). The user's kit settings can change the transmitted notes. Rim entries support modules/pads that actually transmit them; this does not add rim sensors to single-zone toms.

| Nitro trigger | MIDI note(s) | Standard build | Optional Pro build |
|---|---|---|---|
| Kick | 36 | Kick | Kick |
| Snare / rim | 38, 40 | Red | Red pad |
| Tom 1 / rim | 48, 50 | Yellow | Yellow pad |
| Tom 2 / rim | 45, 47 | Blue | Blue pad |
| Tom 3 / rim | 43, 58 | Green | Green pad |
| Hi-hat closed / open / half-open | 42, 46, 23 | Yellow | Yellow cymbal |
| Ride | 51 | Blue | Blue cymbal |
| Crash 1 / Crash 2 | 49, 57 | Green | Green cymbal |
| Hi-hat pedal / splash | 44, 21 | Ignored | Ignored |

All note assignments are in **[include/config.h](include/config.h)**. Add, remove or change table entries and rebuild; each note should appear once (first match wins). Pedal/splash are deliberately not gameplay strikes. Optional expansion tom notes 41/39 are not mapped. Choke/aftertouch and CC4 do not generate hits. Receive channel defaults to omni; set `MIDI_CHANNEL` to 10 if accompaniment or another channel causes extra hits. Stop built-in songs/metronome during diagnosis. Save changed MIDI note assignments in the Nitro as described in its manual.

The parser handles Note On, Note Off, zero-velocity Note On, channel running status, interleaved realtime bytes and system/SysEx boundaries. Values 1–127 become strikes; Note Off/velocity zero are parsed but do not cancel a queued percussive strike. This prevents the module's gate duration from truncating hits. Unknown notes and other message types are safely ignored. System Reset clears parser running status; active percussion pulses expire normally.

## USB identity and timing

The firmware derives the PS3 drum protocol from [Santroller](https://github.com/Santroller/Santroller), pinned in [dependencies.lock.json](dependencies.lock.json). It emulates Santroller's **PS3 Rock Band drum peripheral identity**, VID **12BA**, PID **0210**, device revision **0200**. This is drum emulation, not a generic gamepad or USB MIDI device. Strings identify this project as `Nitro Pico Open Source` / `Rock Band Drums`; no serial string is used.

There is one HID interface with interrupt IN 0x81 and OUT 0x01, 64-byte endpoint buffers, **1ms polling interval**, and a **27-byte input report without a report ID**. The 148-byte HID report descriptor is unchanged from upstream. Feature GET_REPORT ID 0 returns the eight-byte drum initialization response `21 26 01 05 00 00 00 00`. See [docs/protocol.md](docs/protocol.md) for byte offsets, comparison evidence and deliberate differences from the larger Santroller firmware.

MIDI arrives via an interrupt-fed 1024-byte queue. Each logical lane has a bounded 32-hit queue. Main-loop code owns drum state; USB sends combined state for all lanes. A strike stays pressed for at least 4ms **after its IN transfer completes**, followed by an acknowledged release held at least 1ms. Repeated hits are queued rather than extending one continuous button press. A busy USB endpoint cannot silently consume a pending strike/release. Notes a few MIDI bytes apart can overlap in one report (red+yellow+kick, blue+green). No intentional chord collection delay is added.

Default hold values are in `config.h`. Shorter holds may disappear between game samples; longer holds reduce the maximum roll rate and can add queue latency. The host simulation passes 100 strikes/second on one lane with alternating second-lane hits, but that is not a PS3/game performance guarantee. Queues are bounded: unbounded traffic or a stalled host can overflow. Debug counters expose this. Disconnect/suspend discards pending gameplay events so they cannot replay after reconnection.

## Build

The included artifacts are ready to flash. Rebuilding requires Git, Python 3, CMake >=3.20, Ninja, a native C/C++ compiler, and a **complete Arm GNU arm-none-eabi toolchain with newlib**. Tested compiler: **Arm GNU 14.2.Rel1**. Homebrew's bare `arm-none-eabi-gcc` formula lacks libc/newlib and is insufficient by itself.

On macOS, install Xcode Command Line Tools for the native compiler, and `brew install cmake ninja`. Download/extract the appropriate complete [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain). This workspace already contains the tested Apple Silicon toolchain under `.deps/`. Its archive URL and SHA-256 are locked. On Linux install Git, Python, CMake, Ninja and native GCC/G++, then the complete Arm toolchain. On Windows put Git, Python, CMake, Ninja and native GCC/G++ (e.g. MSYS2 UCRT64) on PATH; use the Windows Arm toolchain. The PowerShell wrapper is provided but was not executed on Windows.

```sh
./build.sh --toolchain /absolute/path/to/arm-gnu-toolchain
# With PICO_TOOLCHAIN_PATH set, or the local pinned toolchain present:
./build.sh
./build.sh --debug
./build.sh --pro
```

```powershell
.\build.ps1 --toolchain C:\tools\arm-gnu-toolchain
.\build.ps1 --debug --toolchain C:\tools\arm-gnu-toolchain
```

The scripts fetch exact Pico SDK 2.2.0, TinyUSB and picotool revisions on the first run, compile official picotool without USB/signing support (conversion only), run native tests, compile the firmware and audit ELF/UF2. They refuse unexpected dependency revisions/modified SDK files. Internet is needed for first fetch; normal repeated builds use the local dependencies. No automatic dependency upgrade occurs. No Santroller Configurator is needed or supported by this dedicated adapter.

Outputs are in `dist/standard`, `dist/standard-debug`, or `dist/pro` (`dist/pro-debug` when both flags are selected). A failed build exits nonzero. Existing dist files can remain after failure; only trust a run that ends with `BUILD VERIFIED` and inspect its checksums. Firmware source dependencies are pinned; matching compiler/tool versions are needed for reproducibility. See the verification document for tested scope and byte-rebuild checks.

Native tests only (after the first full build prepares reference headers):

```sh
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

## Connect and play

1. Power off while completing/checking the isolated MIDI circuit.
2. Flash the **standard** UF2. Connect Nitro **MIDI OUT** to adapter MIDI IN.
3. Turn on Nitro, select a kit and stop playback. Connect Pico USB to a PS3 USB port.
4. Launch Rock Band using a normal PS3 controller. Join drums with optional Start or available game navigation. Optional buttons are recommended for standalone menu use.
5. Select standard drums, start an easy song or practice section, and check each color/kick before trying rolls and chords.
6. Execute [docs/ps3-test-plan.md](docs/ps3-test-plan.md) for each of Rock Band, Rock Band 2 and Rock Band 3 you own.

Expected behavior is a drum slot/instrument in Rock Band without console modifications. A lack of XMB navigation or a PS button prompt is not a reliable drum-enumeration test. Actual acceptance, menu joining without optional buttons, sustained roll performance and velocity response remain hardware/game tests.

## Diagnose MIDI and missing/double hits

Flash `dist/standard-debug/nitro_ps3.uf2`. Connect GP4 (physical 6) to **RX** of a **3.3V TTL** USB-UART adapter, and Pico GND to its GND; leave the adapter's power and TX wires disconnected. Open its serial port at **115200, 8-N-1** on a separate computer while Pico USB stays connected to the PS3. The firmware does not add USB CDC or alter the HID identity. Logs are queued and drained only while UART TX has room; a slow console drops debug text, not deliberately blocks gameplay.

Example event: `123456 ch=10 note=38 vel=100 on`. Releases are logged as `off`. Once per second, counters show `rx_overflow`, `uart_errors`, `hit_overflow`, `logdrop`. On a PC, Linux `lsusb -d 12ba:0210 -v` can inspect enumeration; a generic gamepad tester may label instrument axes incorrectly. Inspect raw 27-byte HID reports or USB captures for a reliable protocol check.

| Symptom | Checks |
|---|---|
| No USB device | Data cable, Pico power, flashed UF2, USB port; check BOOTSEL recovery. |
| USB present, no drum slot | Use standard image; record game/region/PS3 version; compare IDs and feature response. Follow test plan; do not install PS3XPAD. |
| No MIDI logs | Nitro OUT vs IN, cable continuity, DIN solder/mating view, optocoupler orientation, 5V supply, 3.3V pull-up, GP1. |
| UART errors | Scope GP1 for idle ~3.3V and clean 32µs bit periods; check isolation, pull-up, base resistor, ground on Pico side. |
| One pad missing/wrong color | Log actual note/channel; compare `config.h`; rim/open-hat assignments and saved module kit may differ. |
| Doubles | Look for two incoming Note Ons, rim+head triggering, cross-talk or splash. Adjust Nitro trigger threshold/cross-talk/retrigger settings before adding firmware filtering. |
| Rolls merge or drop | Compare MIDI count, overflow counters and raw USB rising edges. Try longer hold for game sampling, shorter hold for backlog; change one variable at a time. |
| Extra kicks/colors | Stop module song/demo; set channel 10; inspect input notes. Pedal 44/splash 21 are ignored by default. |
| Pro cymbal/menu oddities | Reflash standard. Pro shares D-pad and velocity fields and has representational limitations. |

No arbitrary MIDI velocity threshold or debounce discards soft hits. Unknown mappings should be diagnosed with serial logging before changing electrical hardware or timing.

## Optional features and limits

P0 standard drums is the default artifact. P1 `--pro` uses upstream pad/cymbal flags, hat classification and inverted velocities; it is experimental and needs RB3 Pro settings/tests. Some simultaneous same-color pad/cymbal/red combinations cannot be represented by the adapted upstream mapping and are documented in [protocol.md](docs/protocol.md). P2 Start/Select/D-pad GPIOs are implemented and compile into all builds. P3 remapping is source-based only; there is no runtime editor or persistence UI.

## Licensing and provenance

The firmware and adapted protocol are GPL-3.0-only; see [LICENSE](LICENSE), [NOTICE.md](NOTICE.md), and [research](docs/research.md). Santroller supplies the established PS3 drum wire format. Pico SDK and TinyUSB retain their own permissive notices. Vendored Santroller source snapshots permit offline comparison. No Roll Limitless code, binary, protocol extraction or dependency was used. USB identifiers are compatibility identifiers, not an allocation granted to this project; this is not an official Sony/Harmonix/Alesis product. When distributing binaries, provide corresponding source and dependency/build information under their licenses.
