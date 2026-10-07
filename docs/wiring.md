# Isolated 5-pin MIDI IN wiring

This is a conventional current-loop MIDI receiver based on [MIDI Association CA-033, figure 2](https://midi.org/wp-content/uploads/wpforo/default_attachments/1709416667-ca33-MIDI-10-Electrical-Specification-Update.pdf), with a 6N138 output powered at 5V and pulled up separately to the Pico's 3.3V rail. The 6N138's output is an open collector. **Do not pull GP1 up to 5V.** A 6N138 should not be assumed specified for a 3.3V VCC supply.

## Schematic (logical connections, not connector geometry)

```text
ISOLATED MIDI/CABLE SIDE                     PICO / USB SIDE

DIN pin 4 -- R1 220 ohm --+-- U1 pin 2 (LED anode)
                         |          : optical isolation :
                      D1 cathode    :                   :     USB VBUS (Pico 40)
                         |          :                   :          |
                      D1 anode      :                   :       U1 pin 8 VCC
                         |          :                   :          |
DIN pin 5 ---------------+-- U1 pin 3 (LED cathode)      C1 100nF
                                    :                   :          |
                                    :                   :       U1 pin 5 GND -- Pico GND (3)
                                    :                   :
Pico 3V3 OUT (36) -- R2 1k ohm --+----------------------- U1 pin 6 OUT
                                |
                                +----------------------- GP1 / UART0 RX (Pico 2)

U1 pin 7 BASE -- R3 4.7k ohm -- Pico GND
U1 pins 1 and 4: not connected
DIN pins 1, 2 and 3, and socket metal shield: not connected
D1 = 1N4148; stripe/cathode toward U1 pin 2 (opposes the internal LED)
```

There is **one 220Ω input resistor** in this receiver. The two transmitter resistors belong inside the Nitro MIDI OUT and must not be added again here. R2 and R3 are different components on the Pico side. Keep the 100nF capacitor close to U1 pins 8 and 5.

## Exact netlist / simple schematic specification

| Ref / net | Connections |
|---|---|
| J1 | 5-pin 180-degree female DIN MIDI IN socket |
| R1, 220Ω 5%, 0.25W | J1.4 → U1.2 |
| INPUT_RETURN | J1.5 → U1.3 → D1 anode |
| INPUT_ANODE | R1 output → U1.2 → D1 cathode (striped end) |
| U1 | 6N138 DIP-8 (check the actual manufacturer's datasheet) |
| VBUS | Pico physical 40 → U1.8 → C1 terminal 1 |
| GND | Pico physical 3 → U1.5 → C1 terminal 2 → R3 terminal 2 |
| RX | U1.6 → GP1 physical 2 → R2 terminal 1 |
| 3V3 | Pico 3V3 OUT physical 36 → R2 terminal 2 |
| R2 | 1kΩ pull-up from RX to 3V3 |
| R3 | 4.7kΩ from U1.7 to GND |
| C1 | 100nF ceramic |
| D1 | 1N4148 reverse-input protection |
| NC | J1.1, J1.2, J1.3, shield, U1.1, U1.4 |

The optocoupler straddles the isolation boundary. Do not connect either DIN signal, pin 2 shield return, or Nitro ground to Pico ground. The CA-033 optional RF capacitors are omitted in this prototype.

## Do not mirror the DIN socket

The schematic above **does not show pin positions**. Use the socket manufacturer's drawing or molded pin numbers to identify J1.4 and J1.5. The **mating/front view looks into the socket from the cable side**; the **rear/solder view looks at it from inside the enclosure**. These views mirror each other. A PCB footprint can additionally be drawn from the board top or bottom. Do not infer solder-lug order from an unlabeled online front-view diagram. With power disconnected, check continuity from the socket's numbered mating contacts to the solder lugs. A standard MIDI cable connects 4→4, 5→5, 2→2.

For the DIP-8 6N138 only, viewed **from above, lettering visible, notch at top**:

```text
             notch
         +----U----+
 NC    1 |         | 8 VCC (5V)
 LED+  2 |  6N138  | 7 BASE (4.7k to GND)
 LED-  3 |         | 6 OUT (GP1; 1k to 3.3V)
 NC    4 |         | 5 GND
         +---------+
```

Do not substitute a 6N137 or H11L1 pin-for-pin without following its datasheet; output/enable requirements differ. Manufacturer pinout and transfer-time specifications take precedence over a generic part name.

## Electrical basis and acceptance checks

CA-033 specifies the isolated current loop, 220Ω receiver resistor, antiparallel protection diode, 31250 baud 8-N-1, and disconnected MIDI IN ground. It requires receiver rise/fall times below 2µs. The 6N138 is explicitly identified as an acceptable receiver family, but resistor values and actual parts affect speed. R2=1k and R3=4.7k are prototype component selections to be confirmed on the assembled hardware, not an assertion of certified compliance. See the [manufacturer-origin 6N138/139 datasheet](https://www.mouser.com/datasheet/2/427/VSMIS13666_1-2572929.pdf) for open-collector pinout and 4.5–5.5V characterization. Buy a traceable part rather than relying on an unknown breakout's pull-up voltage.

1. Before powering, check isolation and continuity against the netlist; verify diode/IC orientation.
2. With USB connected and MIDI disconnected, measure VBUS near 5V, 3V3 near 3.3V, and GP1 idle near 3.3V. Disconnect if GP1 is pulled to 5V.
3. With Nitro sending hits, inspect GP1 using a scope/logic analyzer referenced to **Pico GND only**. Expect idle high, start bit low, 32µs bit cells, and non-inverted UART decoding at 31250 8-N-1.
4. Confirm low/high levels and rise/fall times against CA-033 and the Pico input limits. If slow, check R2/R3 and the exact optocoupler datasheet; do not compensate for malformed electrical signals in MIDI parsing.
5. Confirm debug `uart_errors` and `rx_overflow` remain zero during a dense pattern.

Power Pico from its USB connector. Do not connect an external 5V supply or a debug adapter's VCC to VBUS while PS3 USB is attached. The diagnostic adapter needs only GP4→RX and Pico GND→GND.
