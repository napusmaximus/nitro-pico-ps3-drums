# USB-only drum modules: read before buying parts

**The supplied Pico firmware receives 5-pin DIN MIDI through the isolated GP1 receiver. It does not receive USB MIDI directly.** Its Micro-USB port is reserved for PS3 drum-controller output. The route below adds an external USB-MIDI host; it does not add native USB-host support to the Pico.

## Identify the actual module

“Mesh” describes the drum heads as well as appearing in kit names. Read the model printed on the module and inspect its connectors before buying anything.

- If it has a round 5-pin **MIDI OUT**, use that socket and the existing [beginner build guide](shopping-list-and-build-guide.md). Having a USB port as well does not prevent using DIN MIDI.
- If it has **only USB MIDI**, use the external-host route below.

The [original Nitro module manual](https://www.alesis.com/rscdn/1886/documents/Nitro%20Drum%20Module%20-%20User%20Guide%20-%20v1.2.pdf) documents DIN and USB connections. The [Nitro Max quickstart](https://www.alesis.com/rscdn/2102/documents/Nitro%20Max%20Module%20-%20Quickstart%20Guide%20-%20v1.1.pdf) documents USB-B MIDI. Alesis describes Nitro Max USB MIDI as class-compliant in its [official FAQ](https://support.alesis.com/support/solutions/articles/69000845703-alesis-nitro-max-kit-frequently-asked-questions). This supports considering a class-compliant host; it is not a physical compatibility test of this complete setup.

## Shopping-list additions for USB-only input

Keep **all existing Pico and isolated DIN receiver components**, including the DIN socket, 6N138 circuit and male-to-male MIDI cable. Add:

| Quantity | Buy | Check before ordering |
|---|---|---|
| 1 | **Standalone USB-MIDI HOST with 5-pin DIN MIDI OUT** | Must accept a class-compliant USB MIDI device without a computer and route its notes to DIN OUT. Search `standalone USB MIDI host class compliant DIN MIDI OUT`. |
| 1 | Manufacturer-specified host power supply/cable, if not included | The host needs its own specified power source. Do not power it from Pico header pins. |
| 1 | **USB-A to USB-B data cable** for a module with a square USB-B socket | Often called a printer cable. Confirm the module's actual connector; this is separate from Pico's USB-A to Micro-USB cable. |

One documented example is the **[Kenton MIDI USB Host mk3](https://kentonuk.com/product/midi-usb-host-mk3/)**. The manufacturer describes standalone conversion for class-compliant USB MIDI devices and lists an included supply. Check the current product listing and [manual](https://kentonuk.com/wp-content/uploads/2019/05/mhstman3.pdf), particularly when buying used. Other products must explicitly support this same host-to-DIN-OUT function. No particular Nitro/host/PS3 combination has been tested here; confirm support for your exact module with the host vendor before spending money.

A generic “USB MIDI interface” often expects a computer to be its host. It is **not** interchangeable with a standalone USB-MIDI host. A passive USB adapter, USB hub, USB splitter, OTG cable or USB-to-serial adapter does not perform this conversion. Do not connect USB data wires to GP1 or the 6N138 input.

## Connection and power

```text
Drum module USB MIDI port (USB-B on Nitro Max)
      |
      | USB-B to USB-A DATA cable
      v
Standalone USB-MIDI HOST: USB device input / USB-A host port
      |
      | Host's round 5-pin MIDI OUT
      v
5-pin male-to-male MIDI cable
      |
      v
DIY adapter DIN MIDI IN -> 6N138 receiver -> Pico GP1
                                              |
                                              | Pico Micro-USB DATA cable
                                              v
                                         Stock PS3 USB

Power:
  Drum module <- its own Alesis supply
  USB-MIDI host <- its manufacturer-specified separate supply
  Pico and isolated receiver <- PS3 USB
```

The host's USB-A port connects to the **drum module**, not to the Pico or PS3. The host's **DIN OUT** connects to the DIY adapter's DIN IN. Leave the host's DIN IN unused. Do not use an A-to-A cable to join USB host ports. Do not connect the module simultaneously to the computer and the standalone host using a splitter.

The optocoupler still isolates the DIN input from the Pico side. Do not add a ground wire from the host or drum module to Pico GND, and do not connect DIN pin 2/shield to Pico GND. Pico's debug adapter continues to use Pico-side GND only.

## Sequential setup

1. Identify the module and confirm class-compliant USB MIDI support in its documentation. Confirm the chosen host supports the module before ordering.
2. Follow the beginner guide to flash **[standard UF2](../dist/standard/nitro_ps3.uf2)**, assemble the unchanged isolated receiver, and check approximately 5V at the 6N138 supply and **3.3V at GP1**. Never skip the voltage checks.
3. Turn the drum module and host off and disconnect Pico USB while making connections.
4. Connect the drum module's USB MIDI socket to the host's USB-A device connection using the appropriate data cable.
5. Connect **host MIDI OUT → DIY adapter MIDI IN** with the DIN cable.
6. Power the module with its own supply and the host with its specified supply, following the host manual's startup/connection sequence. Select the host's normal USB-to-DIN routing if configuration is required. Check its documented device-detection indication.
7. Before PS3 testing, strongly prefer the beginner guide's **[standard-debug UF2](../dist/standard-debug/nitro_ps3.uf2)** procedure. Connect Pico USB to the computer and read GP4 through the optional serial adapter. Verify actual notes from every pad, including open/closed hi-hat, and check UART/error counters.
8. Reflash **standard**, then connect Pico USB directly to PS3. Keep the module and host on their separate supplies. Launch Rock Band with a normal controller and follow the [PS3 test plan](ps3-test-plan.md).
9. Record the exact module model, host model/firmware, cable types, emitted note numbers, and game/PS3 version. Test individual inputs, chords, rolls and reconnection through the complete chain.

The supplied note map is based on the original Nitro module. **USB transport does not guarantee the same notes on another module.** Compare serial output with [config.h](../include/config.h); if assignments differ, remap and rebuild using the README instructions or configure the module where supported. Pedal/splash remain ignored by default. There is no automatic module detection or remapping in the supplied firmware.

## If no notes arrive

First determine which link failed: module USB enumeration at the host, host USB-to-DIN routing, or the DIY DIN receiver. Check the host manual's indicators, its separate power supply, the correct USB data cable, and that you used **host DIN OUT**, not IN. Testing module USB MIDI on a computer can establish that the module/cable works, but does not validate the standalone host. Then use the existing receiver voltage/debug checks. Keep tests hub-free initially.

A host may introduce buffering, filtering or note-routing differences. Exercise rolls and chords on the final chain; the existing software tests do not measure this external hardware. **BUILD VERIFIED applies to the unchanged Pico firmware; this USB-host route is HARDWARE NOT VERIFIED.**

## Why no firmware change in this route?

The external host converts USB MIDI to the 31250-baud DIN MIDI stream the current parser already accepts. Thus the supplied UF2, USB identity, Pico GPIOs and input circuit remain applicable. Direct module-USB-to-Pico operation would instead require a second USB host interface, safe host-port power hardware, a USB-MIDI host driver and new integration/testing. That alternative is not implemented or claimed by this update.
