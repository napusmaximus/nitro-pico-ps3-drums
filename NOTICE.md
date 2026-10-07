# Licensing and attribution

This project is released under GPL-3.0-only (see LICENSE). New source: Nitro Pico PS3 Drums contributors, 2026.

The PS3 descriptor macros, report definitions and protocol/mapping derivation come from **Santroller / sanjay900 and contributors**, GPL version 3, https://github.com/Santroller/Santroller, commit `272bae8d28e17e9886e73344adeb8788a062eef4`.

`third_party/santroller/hid_reports.h`, `ps3.hpp`, `ps3_device.cpp` and `rock_band_mappings.cpp` are unmodified reference snapshots. `ps3_descriptor.h` contains unmodified selected macro definitions with an added attribution/pragma. The new `core.c` and `usb_descriptors.c` adapt the drum-specific behavior and wire layout; `main.c` implements a new MIDI/queue/USB completion scheduler. The firmware is not an official Santroller release and does not implement its configurator protocol.

Pico SDK: Raspberry Pi (Trading) Ltd. and contributors, BSD-3-Clause and individual file notices; see `third_party/licenses/pico-sdk.txt` and the locked dependency sources. TinyUSB: Ha Thach and contributors, MIT; see `third_party/licenses/tinyusb.txt`. Official picotool is a build-time tool, not linked into the firmware. The Arm toolchain supplies newlib 4.4.0 and libgcc runtime components under their respective upstream licenses/runtime exceptions (see `third_party/licenses/newlib.txt` and `gcc-runtime-exception.txt`); the downloaded toolchain and release manifest are retained under `.deps/`. Obtain corresponding toolchain sources/notices from the official Arm release when redistributing a toolchain or source bundle.

Provide the complete corresponding project source, build scripts, dependency lock and applicable upstream sources/notices with redistributed GPL binaries. No proprietary Roll Limitless firmware or extracted material is included. Names and USB identifiers describe compatibility and do not imply affiliation, certification or an assigned vendor ID.
