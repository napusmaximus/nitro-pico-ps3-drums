# First boot and acceptance test: unmodified PS3

**USB-only module:** the Pico still requires this isolated DIN receiver. Use [USB-only setup](usb-midi-setup.md) to connect module USB to a separately powered standalone USB-MIDI host, then **host DIN OUT → adapter DIN IN**. References below to Nitro MIDI OUT apply to host DIN OUT on that route. Do not connect module USB directly to Pico. Record the host model and verify actual pad notes before PS3 testing.

Status: **not executed on hardware**. Use one row per game/version and preserve failures as well as passes. Required: assembled adapter, Pico, Nitro Mesh, 5-pin cable, USB data cable, stock PS3, original game and a normal PS3 controller. Optional: six navigation switches, scope/logic analyzer, 3.3V USB-UART adapter, PC for raw USB captures.

## 1. Record the setup

Record date; UF2 path and SHA-256; Pico model; optocoupler manufacturer/part; resistor values; PS3 model/stock firmware; game title, disc/product ID, region and update version; Nitro kit number and saved MIDI changes. Confirm the console has no PS3XPAD/HEN/CFW dependency. Begin with `dist/standard/nitro_ps3.uf2`.

## 2. Electrical preflight

1. USB and Nitro disconnected: compare each net against `wiring.md`. Confirm the DIN socket's actual pin numbers from its own drawing. Check no DC connection from DIN 2/4/5 to Pico GND.
2. Power Pico by USB alone. Check U1.8≈5V, pull-up≈3.3V, RX idle≈3.3V. Stop and fix if GP1 is at 5V or stuck low.
3. Flash standard-debug first if MIDI diagnosis is needed. Attach only GP4→USB-UART RX and Pico GND→adapter GND at 115200 baud. Do not attach its VCC/TX.
4. Connect Nitro MIDI OUT, power Nitro, stop songs/demos and hit each pad. Compare notes/velocities against the README table, including open/closed hat and rim. Release messages should not generate a second hit. Verify electrical edges and zero UART error/overflow counters under dense playing.
5. If debugging on a PC, inspect `12ba:0210`, one HID interface, 27-byte input, 1ms endpoint interval and neutral hat=8. Capture feature ID0 if requested. A generic browser gamepad label is not authoritative.

## 3. Stock-PS3 enumeration (P0 gate)

1. Flash the standard image after circuit diagnosis. Fully stop the game; connect Pico directly to a PS3 USB port, without a hub for the first run.
2. Launch the game with a normal PS3 controller. Observe whether a drum instrument/player slot is available. Try Start on the optional GP6 switch to join. Record exactly what appears; distinguish no enumeration from a menu-joining problem.
3. Keep cymbal/Pro options disabled. Do not rely on XMB control, PS/Home functionality or a homebrew tester. Do not install a plugin to make this step pass.
4. If drums are not recognized, try a different data cable/port, cold boot with the adapter connected, and launch with it connected after boot. Collect raw enumeration on a PC and, if available, a USB bus analyzer on PS3. Compare against `protocol.md`. Record failure rather than declaring compatibility.

**Pass:** the unmodified console/game lets this device join as drums without software modifications or another instrument supplying authentication.

## 4. Single-hit and mapping tests

Use a practice/freestyle area or a predictable easy song with visible drum feedback.

1. Make 20 deliberate hits each on kick, snare, rim, tom1, tom2, tom3, closed hat, open hat, crash and ride. Record expected color and registered count.
2. Confirm pedal closure/splash alone produces no game strike. Verify no unexplained menu navigation or stuck lanes.
3. Test soft/medium/hard strikes; record whether gameplay accepts soft hits and whether velocity affects fills/freestyle (not all gameplay uses velocity).
4. Stop playing for 10 seconds; no button/lane should remain pressed. Briefly unplug the MIDI cable and reconnect; idle must remain neutral.

**Pass:** all configured sources produce the documented color/kick with no missing or duplicate game strikes in the test. Diagnose discrepancies by actual MIDI notes first.

## 5. Chords and rolls

1. Repeat red+yellow+kick together 30 times at a slow rate. Repeat blue+green together 30 times. Test crash+kick and hat+snare+kick patterns.
2. Play snare rolls at 4, 8, 12, 16 and 20 hits/second for 10 seconds each, as physically feasible. Start with a metronome or controlled MIDI source if counts matter; separate human timing errors from adapter loss.
3. Play alternating snare/hat patterns, double kicks, and tom/crash on the same standard color. Check each attack is separated rather than becoming one held input.
4. In the debug image, counters must remain zero except `logdrop` may rise if verbose diagnostics are saturated. Compare input Note On count with raw USB rising edges; game judgement alone conflates input loss with chart timing.
5. Repeat with holds changed only if needed. Record settings, count and result. Never assume the 100Hz host simulation means the game accepts that rate.

**Pass:** expected inputs register over sustained real playing without unexplained loss/doubles, and no UART/hit queues overflow under the target playing workload.

## 6. Lifecycle and navigation

1. Play for at least 75 minutes to cross the microsecond timer wrap. Check rolls immediately around the wrap if a timestamped capture is available.
2. Disconnect/reconnect Pico USB, leave/re-enter the game and cold-boot PS3 with Pico attached. Queued old hits must not replay after reconnect.
3. Test Start, Select and all D-pad directions/diagonals. Opposing directions should neutralize; switches must not chatter. Check menu join/pause without requiring a PS button on this adapter.
4. Repeat initial recognition and a short gameplay test independently in Rock Band, Rock Band 2 and Rock Band 3. Record each version; do not infer all-game compatibility from one pass.

## 7. Optional Pro Drums (P1, only after P0 passes)

1. Flash `dist/pro/nitro_ps3.uf2`; enable RB3 Pro drums with the appropriate cymbals.
2. Check yellow/blue/green tom pads versus hi-hat/ride/crash cymbals separately, 20 hits each.
3. Test two different cymbals, cymbal+kick, cymbal+snare, and a same-color pad+cymbal pair without snare. Test unequal velocities and equal-velocity yellow+blue cymbals.
4. Record limitations from `protocol.md`: red plus same-color pad+cymbal, multiple shared velocity pairs, and D-pad classification conflicts. Do not certify these combinations supported without further implementation/testing.
5. Return to standard UF2 if Pro interferes with basic play.

## Results sheet

| Game/version/region | Stock enumeration | Single hits | Chords | Rolls | Reconnect | Pro (RB3) | Capture/notes |
|---|---|---|---|---|---|---|---|
| Rock Band | NOT TESTED | | | | | N/A | |
| Rock Band 2 | NOT TESTED | | | | | N/A | |
| Rock Band 3 | NOT TESTED | | | | | NOT TESTED | |

Only mark **HARDWARE VERIFIED** for the exact setup and cases actually passed. Attach the firmware hash, circuit values and test observations to any bug report.
