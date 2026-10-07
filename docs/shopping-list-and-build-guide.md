# Shopping List and Beginner Build Guide

This guide takes you from a box of parts to testing an Alesis Nitro Mesh drum kit with Rock Band on an unmodified PlayStation 3. No electronics experience is assumed. Read each section before doing it, and finish each checkpoint before continuing.

**BUILD VERIFIED. HARDWARE NOT VERIFIED.** The firmware has compiled and passed software checks; the circuit and PS3 gameplay still need physical testing. Buying these parts is not a guarantee of compatibility with your particular game/version. See the [verification record](verification.md). Start with **standard drums**, then consider experimental Pro Drums after the basic adapter works.

You do not need to compile software for this tutorial. The three UF2 files are already in this repository. A **UF2** is the firmware file you copy onto the Pico to tell it what to do.

## 1. Shopping list: what to buy

You also need your existing Nitro Mesh kit and its power adapter, a stock PS3, a Rock Band game, a normal DualShock/Sixaxis controller, and a computer for downloading and flashing firmware. A computer is not required during normal play.

### Required parts

Buy through a reputable electronics distributor or an official reseller where possible. Search phrases describe the required type, not a guarantee that every search result is suitable. Check the listing's package, voltage and connector details before ordering. The [official original Pico product page](https://www.raspberrypi.com/products/raspberry-pi-pico/) provides the original board family and reseller information; choose the **Pico H with RP2040 and already soldered headers**.

| Quantity | What to buy | Why it is needed | Suggested search phrase |
|---|---|---|---|
| 1 | **Original Raspberry Pi Pico H, RP2040**, with both long header strips already soldered | Runs firmware; the H version avoids soldering headers yourself | `Raspberry Pi Pico H RP2040 original pre soldered headers` |
| 1, preferably 2 including spare | **6N138, DIP-8 through-hole** optocoupler | Transfers MIDI through light inside the chip, keeping the drum-kit circuit electrically separate | `6N138 DIP-8 optocoupler` |
| 1 | **5-pin 180-degree female DIN socket**, with identifiable numbered contacts | Receives the male MIDI cable plug | `5 pin DIN female MIDI socket 180 degree` |
| 1 if needed for the socket | **Passive DIN socket breakout with screw terminals**, already assembled, or a socket with already attached, labeled wires | Makes the socket usable without soldering; most bare socket lugs do not fit a breadboard | `5 pin DIN female 180 degree passive screw terminal breakout` |
| 1 | Standard **5-pin male-to-male DIN MIDI cable** | Connects Nitro MIDI OUT to the new socket | `5 pin DIN male to male MIDI cable` |
| 1 | **Full-size solderless breadboard**, roughly 830 tie points, 2.54mm hole spacing, a–j rows and center trench | Holds and connects the receiver components without solder | `830 point solderless breadboard` |
| 1 pack each | **Male-to-male and female-to-male 2.54mm jumper wires**, plus spare wires | Male ends fit breadboard holes; female sockets attach to Pico H header pins | `Dupont jumper wires male male female male 2.54mm` |
| 1 each, plus spares | **220Ω**, **1kΩ**, **4.7kΩ** through-hole axial resistors, 0.25W, 1% or 5% | Limit input current, pull RX to 3.3V, and connect the optocoupler base to ground | `220 ohm 1k 4.7k resistor through hole quarter watt` |
| 1 | **1N4148 through-hole diode** with visible stripe | Protects the optocoupler input from reverse voltage | `1N4148 axial through hole diode` |
| 1 | **100nF / 0.1µF ceramic capacitor**, through-hole, rated at least 16V | Stabilizes the optocoupler's supply | `100nF 104 ceramic capacitor through hole 50V` |
| 1 | **USB-A to Micro-USB DATA cable** | Powers Pico and carries controller data to the PS3; also used for flashing | `USB A to micro USB data sync cable` |

For a fully solderless start, obtain the DIN socket/breakout **already assembled**. A passive breakout only brings the five DIN contacts to labeled terminals; it must not contain a MIDI converter or powered receiver. Check that each terminal number corresponds to the DIN contact number. If you buy a bare solder-lug socket, ask someone experienced to attach insulated leads and label pins 4 and 5 before starting. Jumper sockets pushed loosely onto solder lugs are not dependable connections. Use appropriate wire ends for the screw terminals; do not force oversized jumper pins into them. A wire stripper is useful if the terminal connection needs bare wire.

### Strongly recommended tools

| Item | Why / what to search for |
|---|---|
| Digital multimeter with DC voltage, resistance and continuity modes | Checks connections and prevents applying 5V to a 3.3V signal input. Search `digital multimeter DC voltage continuity resistance`. Treat the voltage checks below as a checkpoint: borrow a meter if you do not own one. |
| Small needle-nose pliers | Gently shape resistor/component leads. Search `electronics needle nose pliers`. |
| Small flush cutters | Trim excess leads when appropriate. Search `electronics flush cutters`. Keep cut pieces away from the circuit and your eyes. |
| Extra jumper wires and paper labels | Replace unreliable wires and label GND, 3V3, 5V, RX and DIN contacts. |
| Small screwdriver, if using screw terminals | Tightens the passive DIN breakout connections. Match the screw size. |

### Optional extras

| Item | Purpose / suggested search phrase |
|---|---|
| USB-to-TTL UART adapter with **documented 3.3V signal levels** and labeled RX/GND | Reads MIDI logs before PS3 testing; strongly recommended for diagnosis. Search `USB UART TTL 3.3V logic RX GND adapter`. A power-voltage jumper alone does not establish signal voltage. Not an RS-232 adapter. |
| 1–6 normally-open momentary pushbuttons, preferably two-terminal or documented breadboard-compatible types | Start is particularly useful for joining a game; Select and D-pad are optional. Search `normally open momentary push button breadboard`. |
| Logic analyzer supporting 3.3V signals, or oscilloscope | An experienced helper can inspect signal timing if voltage checks pass but UART errors persist. |

### Do not substitute these

- **Pico 2 / RP2350:** this firmware is built for the original RP2040 Pico. Buy Pico H, not Pico 2. This guide does not target wireless variants or unrelated RP2040 boards either.
- **USB MIDI adapter or generic MIDI-to-USB cable:** those expose MIDI to a computer; they do not replace the PS3 drum-controller adapter here.
- **Random powered “5V MIDI receiver” modules:** their RX output may be pulled to 5V, which is unsuitable for GP1. Build the specified circuit.
- **PS3XPAD, HEN, CFW or related hardware/software:** none is required for this native-controller test.
- **Surface-mount 6N138, 6N137, different DIN angle/pin count, or charging-only USB cable:** these do not match the assembly instructions.

## 2. Identify the parts before connecting anything

Spread everything on a dry, nonconductive table. Do not work on foil, metal, or a conductive antistatic bag. Keep all USB cables and the MIDI cable disconnected while identifying and assembling parts.

### Pico H: physical pins are not GPIO numbers

The Pico is the narrow green circuit board with a Micro-USB socket at one end and a small BOOTSEL button. The original Pico H has two rows of metal header pins already attached. The large chip is RP2040.

Look at the **component side from above, USB socket at the top**. Physical pin 1 is at the top left; count down the left side to pin 20. Physical pin 21 is bottom right; count up the right side to pin 40 at the top right. Turning the board over mirrors this view. The printing on the board and the [official Pico datasheet](https://datasheets.raspberrypi.com/pico/pico-datasheet.pdf) help confirm orientation.

```text
             Micro-USB
           +------------+
 physical 1|            |40 physical = VBUS
          2| GP1        |39
          3| GND        |38
           |    ...     |37
           |            |36 = 3V3 OUT
           |    ...     |
         20|            |21
           +------------+
     TOP / COMPONENT VIEW, USB at top
```

| Physical header pin | Printed/function name | Use here |
|---|---|---|
| 2 | GP1 / UART0 RX | Incoming MIDI from optocoupler output |
| 3 | GND | Pico-side ground, the reference for voltage measurements |
| 6 | GP4 / UART1 TX | Optional outgoing debug text |
| 9 | GP6 | Optional Start |
| 10 | GP7 | Optional Select |
| 11 | GP8 | Optional D-pad Up |
| 12 | GP9 | Optional D-pad Down |
| 14 | GP10 | Optional D-pad Left |
| 15 | GP11 | Optional D-pad Right |
| 36 | 3V3 OUT | 3.3V supply for the RX pull-up resistor |
| 40 | VBUS | USB's approximately 5V supply for the optocoupler |

**“GP1” means physical pin 2, not physical pin 1.** “6N138 pin 2” refers to the separate eight-leg chip, not the Pico. Always read the component name along with its pin number. Leave other Pico pins alone, including VSYS and 3V3_EN.

### Breadboard: which holes connect?

A breadboard grips component leads with metal contacts hidden underneath. It does not need solder.

On the common a–j board, the five holes **a10, b10, c10, d10, e10** connect to each other. The five holes **f10, g10, h10, i10, j10** form a separate connection. They do **not** connect across the center trench. Row 11 is separate from row 10.

```text
same numbered row:  a---b---c---d---e   TRENCH   f---g---h---i---j
                     one connection              another connection
next row:           a---b---c---d---e            f---g---h---i---j
                     separate from the row above
```

A **node** means all the connected points of one electrical connection. Two wires in the same five-hole group share a node. Both ends of a resistor in the same group would bypass the resistor, so put its ends in different groups as instructed.

Long outside strips marked +/− are **power rails**. The marks do not supply power; they are just labels. Rails may be split halfway along their length, and opposite sides are usually not joined. Confirm their connections with an unpowered continuity test. **The worked layout below does not use the long rails**, avoiding split-rail surprises.

The 6N138 straddles the trench so opposite legs remain separate. Push leads straight into holes gently. Do not insert two leads into one hole. If a part cannot reach, use an extra jumper and a spare, separately checked node; do not force it or allow bare leads to touch.

### 6N138 optocoupler

This is a small black eight-leg chip marked `6N138`. Look from above at its lettering. A semicircular notch identifies one end; a molded dot may identify pin 1. With the notch at the top, pins count down the left, then up the right. Confirm the manufacturer's drawing if markings are unclear; a random mold mark is not a reliable pin-1 indicator.

```text
             notch
         +----U----+
 NC    1 |         | 8 VCC
 LED+  2 |  6N138  | 7 BASE
 LED-  3 |         | 6 OUT
 NC    4 |         | 5 GND
         +---------+
```

NC means “not connected.” Pins 2/3 are on the MIDI side. Pins 5/6/7/8 are on the Pico side. An internal light source carries the signal across the separation; there must be no external ground wire joining the two sides.

### Resistors

A resistor usually has a small cylindrical body, colored bands and one wire at each end. These resistors have no polarity: either end may face either direction. Keep them labeled until installed.

| Value | Equivalent notation | Common four-band colors, with gold tolerance band last |
|---|---|---|
| 220 ohms | 220Ω, 220R | red – red – brown – gold |
| 1,000 ohms | 1kΩ, 1K | brown – black – red – gold |
| 4,700 ohms | 4.7kΩ, 4K7 | yellow – violet – red – gold |

Five-band parts use a different band pattern; do not apply this four-band table to them. Check the packaging or measure each loose resistor in the meter's Ω mode before installation. Read the meter's units: 0.220kΩ is 220Ω. Never measure resistance on a powered circuit.

### 1N4148 diode

The through-hole version often has a small glass body, a wire at each end and a dark stripe near one end. Unlike a resistor, it **does have a direction**. The stripe marks the **cathode**.

- **Striped end → 6N138 pin 2 node.**
- **Unstriped end → 6N138 pin 3 node.**

The diode goes across those two input nodes, not in series with the MIDI wire. Hold leads near the body with pliers when bending, without squeezing or cracking the glass.

### 100nF capacitor

This is a small ceramic disc or dipped component with two leads. **100nF = 0.1µF**, commonly marked **104**. Buy a ceramic version: it is non-polarized, so either lead can go to either of its two specified connection points. Do not mistake 100µF for 100nF.

### DIN socket

> **THE FRONT VIEW AND SOLDER-SIDE VIEW ARE MIRRORED. DO NOT GUESS WHICH LUG IS PIN 4 OR PIN 5.**

The front/mating view looks into the socket where the cable enters. The rear/solder-side view looks at the back of that socket. A manufacturer's PCB drawing may use yet another viewing direction.

Use the **actual manufacturer's datasheet**, **numbers molded into the connector**, or an **unpowered continuity test from an already identified numbered contact** to its lug/terminal. Continuity can trace a known contact; it cannot tell you an unknown contact's number without a reliable reference. Mark the two terminal wires `DIN 4` and `DIN 5` before continuing. If you cannot establish the numbers, get help or a socket with a documented pinout. No positional DIN drawing in this guide overrides your connector's numbering.

## 3. Flash the Pico first, before building the circuit

1. Leave the Pico disconnected from the breadboard and all MIDI/serial wiring.
2. Download **[dist/standard/nitro_ps3.uf2](../dist/standard/nitro_ps3.uf2)**. On GitHub, open the file and use **Download raw file**. Do not save the web page as HTML. Because this repository is private, sign in with an account that has access. Alternatively, download the repository ZIP, extract it and find the file under `dist/standard`.
3. Disconnect the Pico USB cable if attached.
4. Press and hold the little **BOOTSEL** button on the Pico.
5. While holding BOOTSEL, connect the Micro-USB end to Pico and the other end to your computer. The cable must carry **DATA**, not just charging power.
6. Release BOOTSEL. Wait for a removable drive named **RPI-RP2** in your file manager.
7. Copy `nitro_ps3.uf2` from the **standard** folder onto RPI-RP2. You do not copy the ZIP, source folder, ELF or BIN file.
8. The Pico automatically reboots and RPI-RP2 disappears. This is expected, not a failed copy.
9. Disconnect/reconnect normally **without holding BOOTSEL**. It should now appear as a USB controller/HID device rather than a storage drive. A lit LED is not a required success indicator.
10. Check the USB identity on the computer before adding electronics.

Expected identity: **VID 12BA**, **PID 0210**. Manufacturer string: **Nitro Pico Open Source**. Product string: **Rock Band Drums**. The OS may show a generic HID name instead, so use the numbers when available.

- **Windows:** open Device Manager, inspect the newly appearing USB/HID device's Properties → Details → Hardware Ids; look for `VID_12BA&PID_0210`. Unplug/replug to distinguish it from other devices.
- **macOS:** open System Information (search using Spotlight), select USB, and inspect the device for vendor `0x12ba` and product `0x0210`.
- **Linux:** run `lsusb -d 12ba:0210` in a terminal (requires the system's `lsusb` utility).

Do not install a special controller driver merely to check enumeration. “Enumerates” means the computer detects and identifies the USB device. That proves the Pico can run the firmware and talk USB; it does not prove PS3 gameplay works. If this check fails, resolve the USB problem before building the MIDI circuit. Disconnect Pico again when done.

## 4. Build the breadboard circuit, one connection at a time

**ALL POWER DISCONNECTED:** unplug Pico USB, unplug the serial adapter if present, disconnect the MIDI cable and turn Nitro off. Never add or move wires while power is connected.

This is exactly the circuit in [wiring.md](wiring.md), expressed as a worked breadboard layout. It uses an a–j, numbered-row breadboard. If your board is laid out differently, first identify its connected groups with the meter; do not apply these hole names blindly.

### Place the chip and label three supply nodes

Keep Pico **beside** the breadboard on the nonconductive surface. Use female-to-male jumpers: the female socket slips fully onto one Pico header pin; the male end enters a breadboard hole. This avoids having to guess which breadboard holes the wide Pico covers. Support it so header pins cannot touch loose metal or each other.

1. Orient the breadboard with rows increasing downward, a–e left of the trench and f–j right of it.
2. Put the 6N138 notch toward the lower-numbered rows/top. Insert it across the trench in rows 10–13: **pin 1=e10, pin 2=e11, pin 3=e12, pin 4=e13, pin 5=f13, pin 6=f12, pin 7=f11, pin 8=f10**. Seat it gently and check for folded legs. If leg spacing needs adjustment, align gently before insertion rather than forcing it.
3. Label the left five-hole group of row **20 “3V3”**, row **22 “5V/VBUS”**, and row **24 “GND”**. These three rows must remain separate.

### Connect the Pico side

Work through each numbered instruction once, then tick it off on paper. A hole such as j10 connects to f10 because they are in the same five-hole group.

4. Connect **Pico physical 40 (VBUS)** to **a22**. Connect a male-to-male jumper **b22 → j10**. This supplies **6N138 pin 8** with USB 5V.
5. Connect **Pico physical 3 (GND)** to **a24**. Add jumper **b24 → j13**. This connects **6N138 pin 5** to Pico GND.
6. Connect **Pico physical 2 (GP1)** to **j12**, sharing the node with **6N138 pin 6 (OUT)**. This is the RX signal node, not a supply rail.
7. Connect **Pico physical 36 (3V3 OUT)** to **a20**.
8. Put the **1kΩ resistor** between **d20 and d18**, then jumper **a18 → h12**. This connects 3V3 **through the resistor** to the pin-6/GP1 node. It is the “pull-up”: it holds RX near 3.3V when no MIDI signal is present. **Do not connect this resistor to row 22/5V.**
9. Put the **4.7kΩ resistor** between **h11 and h15**, then jumper **j15 → c24**. This connects **6N138 pin 7 (BASE)** through 4.7kΩ to Pico GND.
10. Put the **100nF ceramic capacitor** between **i10 and i13**, connecting **6N138 pin 8 to pin 5**. Either capacitor lead can face either way. Gently space the leads to fit; they must not touch pins/nodes 7 or 6 along the way. Keep it close to the chip. If it cannot reach, extend a connection with an insulated jumper rather than forcing the part.

### Connect the isolated MIDI side

11. Attach the correctly identified **DIN pin 4** lead to **a17**.
12. Put the **220Ω resistor** between **d17 and d11**. This makes **DIN 4 → 220Ω → 6N138 pin 2**. There is just one 220Ω resistor in this receiver input.
13. Attach the correctly identified **DIN pin 5** lead to **a12**, the same node as **6N138 pin 3**.
14. Put the **1N4148 diode across c11 and c12**: **stripe at c11 (pin 2 side)**; **unstriped end at c12 (pin 3 side)**. Bend the leads carefully to suit the spacing; the glass body must not be forced against the chip.
15. Leave **6N138 pins 1 and 4** unconnected. Leave **DIN pins 1, 2, 3 and metal shield** unconnected. Insulate any unused exposed DIN wires separately so they cannot touch other wiring.
16. Re-read every connection from the actual parts, not just your memory. Check that no bare lead crosses onto a neighboring node.

### Connection summary: use this to audit the layout

```text
DIN 4 -- 220 ohm -- 6N138 pin 2
DIN 5 ------------ 6N138 pin 3
1N4148 stripe ---- pin 2 node
1N4148 other end - pin 3 node

Pico physical 40 (VBUS) ----------- 6N138 pin 8
Pico physical 3  (GND) ------------ 6N138 pin 5
Pico physical 2  (GP1) ------------ 6N138 pin 6
Pico physical 36 (3V3) -- 1k ohm -- 6N138 pin 6 / GP1 node
6N138 pin 7 ----------- 4.7k ohm -- Pico GND
100nF capacitor ------------------ between 6N138 pins 8 and 5
```

> **DO NOT connect DIN pin 2 to Pico GND. DO NOT connect Nitro ground to Pico ground. DO NOT connect MIDI directly to GP1.**

Isolation means the incoming MIDI circuit and the Pico circuit have no direct electrical connection. The optocoupler carries the message using light inside the chip. Adding a “helpful” ground wire across that separation defeats its purpose. Pico-side buttons and the debug adapter do share **Pico GND**, as described later; they do not connect to DIN ground.

## 5. Multimeter pre-flight: check before connecting the drums

### Set up the meter

1. Put the **black lead into COM** and the **red lead into V/Ω**. **Do not use the A/mA current sockets or current mode** for any check here; a current measurement across a supply can short it.
2. For continuity, select the continuity/beeper symbol described in your meter manual. Touch probe tips together to learn its beep/near-zero reading. “OL” typically means open/no connection.
3. For resistance, select Ω; for voltage select **DC V**, usually a V with a solid line above a dashed line. Do not choose AC V (`V~`). If not autoranging, select a DC range above 5V, such as 20V.
4. Hold the insulated probe grips. Avoid bridging two adjacent pins with one metal tip. Spare jumper wires can bring a test point to an accessible empty hole.

### Unpowered continuity/resistance checks

Unplug **both USB connections and MIDI**, even if the computer is off. Continuity and resistance checks are made only with power disconnected.

- Confirm the row groupings: a20↔e20 conducts, a20↔a22 does not form a direct wire connection. Verify the chip legs really straddle the trench.
- Check each direct wire: Pico 40↔6N138 8; Pico 3↔6N138 5; Pico 2↔6N138 6; DIN 5↔6N138 3. Each should read near zero ohms/beep.
- Check resistors **before installing**, or lift one leg with power off if necessary: 220Ω, 1kΩ, 4.7kΩ. A continuity beep is not a resistor-value test. In-circuit semiconductor paths can affect resistance readings.
- Check for an accidental direct short from 5V to GND, 3V3 to GND, or 5V to the GP1 node. Components can cause temporary/changing readings; a sustained near-zero reading needs investigation. Do not assume “no beep” alone certifies the circuit.
- Check **DIN 2, DIN 4, DIN 5 and DIN shield against Pico GND**: there must be no direct conductive connection. An unconnected DIN pin should not have a ground jumper. Do not expect isolation between DIN 4 and DIN 5 themselves: that is the intended LED/diode input path.

### Powered DC voltage checks, with MIDI still disconnected

Change the meter to **DC volts**, with leads still in COM and V/Ω. Connect the black probe to **Pico GND**, for example a free hole in the left row-24 group. Keep it there. Connect **Pico USB to the computer**, with no MIDI or debug adapter attached. Carefully touch the red probe to one measurement point at a time.

| Measure red probe here, black at Pico GND | Worked-layout test hole | Expected |
|---|---|---|
| Pico physical 40 / 6N138 pin 8 node | c22 | Approximately **5V** |
| Pico physical 36 / 3V3 OUT | c20 | Approximately **3.3V** |
| Pico physical 2 / GP1 / 6N138 pin 6 | i12 | Idle approximately **3.3V** |

> **IF GP1 IS NEAR 5V, DISCONNECT POWER IMMEDIATELY.** GP1 must be pulled up to **3.3V**, through the 1kΩ resistor. The 6N138 supply is 5V; its output pull-up is a separate 3.3V connection.

If any measurement is wrong, disconnect USB before changing anything. Recheck resistor values, rows, Pico physical pin numbers, chip orientation and diode orientation. A near-zero RX voltage with no MIDI also needs investigation. Do not connect Nitro or PS3 until these checks pass. Disconnect USB again after measuring. A multimeter checks static voltage; it cannot confirm fast MIDI edge timing. Persistent UART errors may need an experienced helper and an oscilloscope, as described in [wiring.md](wiring.md).

## 6. Connect the Alesis Nitro Mesh

With the Pico unplugged and Nitro switched off, plug the male-to-male MIDI cable into the module's **MIDI OUT**, then into your new female DIN socket. The DIY socket is the adapter's **MIDI IN**.

```text
Alesis Nitro Mesh MIDI OUT   (NOT the module's MIDI IN)
             |
             | standard 5-pin male-to-male MIDI cable
             v
DIY adapter MIDI IN socket
```

Use Nitro's own Alesis power adapter. Pico gets power from its USB connection. **Do not join the two power supplies or add a ground wire between the module and Pico.** Turn off built-in songs, demos and metronome for initial tests so you can associate each incoming event with a deliberate hit. Select your usual drum kit; customized kit settings may have changed its MIDI notes.

## 7. Debug MIDI before PS3: optional, strongly recommended

This step shows what the Nitro actually sends. It can save substantial guesswork if a pad later triggers the wrong color. You need the optional **3.3V logic USB-to-TTL UART adapter** and a computer serial-terminal program.

1. Unplug Pico, unplug the serial adapter and turn Nitro off before attaching wires.
2. Repeat the BOOTSEL procedure from section 3, this time flashing **[dist/standard-debug/nitro_ps3.uf2](../dist/standard-debug/nitro_ps3.uf2)**. The filename is the same; check its **folder** carefully. Then unplug Pico again.
3. Connect **Pico physical 6 / GP4 → serial adapter RX**. TX on Pico means “transmit”; RX on the adapter means “receive.”
4. Connect **Pico GND → serial adapter GND**. With the worked layout, an available hole such as d24 can supply this Pico-side ground.
5. **DO NOT CONNECT the adapter's VCC/power pin or its TX pin.** Only RX and GND are connected. Do not connect to a PC's RS-232 connector.
6. Connect Pico's own USB cable to the computer for power and USB enumeration. Separately plug the serial adapter into the computer. Turn Nitro on and stop song/demo playback.
7. Open a serial terminal, select the port belonging to the **USB-TTL adapter** (for example a COM port on Windows or `/dev/cu...` on macOS). Unplug/replug the adapter to identify its port if needed. Use the adapter manufacturer's driver instructions if it is not detected.
8. Set **115200 baud, 8 data bits, no parity, 1 stop bit**, **no hardware/software flow control**. This is often written **115200 8-N-1**. These are debug settings; the MIDI input itself operates at 31250 baud.
9. Open the connection and strike one pad. You should see event lines and a statistics line roughly once a second.

The debug output does **not** appear through Pico's own USB port as a serial console; that port still acts as the drum controller. Use any serial terminal that supports the settings above. If using an installed `screen` on macOS/Linux, `screen /dev/your-adapter-port 115200` is one option; substitute the real adapter port, and use Ctrl-A then `\` to exit. A graphical terminal's Connect/Disconnect controls may be easier for a first attempt.

Example:

```text
123456 ch=10 note=38 vel=100 on
stats rx_overflow=0 uart_errors=0 hit_overflow=0 logdrop=0
```

The first number is a timestamp; `ch` is MIDI channel; `note` identifies the trigger; `vel` is hit strength. `on` means a strike, while `off` is a release. A zero-velocity Note On is treated as off and does not make another strike.

| Strike | Expected default MIDI note | Standard color |
|---|---|---|
| Kick | 36 | Kick |
| Snare | 38 | Red |
| Snare rim | 40 | Red |
| Tom 1 | 48 | Yellow |
| Tom 1 rim, if supported | 50 | Yellow |
| Tom 2 | 45 | Blue |
| Tom 2 rim, if supported | 47 | Blue |
| Tom 3 | 43 | Green |
| Tom 3 rim, if supported | 58 | Green |
| Closed hi-hat | 42 | Yellow |
| Open hi-hat | 46 | Yellow |
| Half-open hi-hat | 23 | Yellow |
| Ride | 51 | Blue |
| Crash | 49 | Green |
| Crash 2, if connected/supported | 57 | Green |

Hi-hat pedal **44** and splash **21** may appear in logs but are intentionally ignored for game hits. Rim mappings do not add rim sensors to a single-zone pad. A different note is not automatically an electrical fault: the kit may have customized assignments. Compare with [config.h](../include/config.h) and the [README mapping table](../README.md#midi-mapping). Changing that file requires rebuilding firmware; merely editing it does not change an already flashed UF2.

Desired counters are **rx_overflow=0, uart_errors=0, hit_overflow=0**. The first means incoming-byte buffer overflow; the second means UART reception errors; the third means the gameplay strike queues filled. `logdrop` counts discarded debug text; it can rise under heavy logging without proving gameplay hits were lost. On a computer that does not poll HID reports, `hit_overflow` may grow despite valid MIDI input; repeat gameplay-queue checks with an active report-reading host or the PS3. Valid note lines verify MIDI reception, not PS3 recognition.

Record the notes for every pad, then close the terminal and unplug USB before changing wiring. Reflash **standard** before the first PS3 test. The debug image can be used again later, with Pico USB connected to PS3 and the serial adapter connected to a computer, using the same two diagnostic wires only.

## 8. Optional Start / Select buttons

**Install at least Start if possible.** It helps join a player slot; the adapter has no PS/Home button. Add these with all power disconnected.

Use a **normally-open momentary** button: its contacts connect only while you hold it down, then spring apart on release. Connect one contact to the specified Pico pin and the other to Pico GND. No external pull-up resistor is needed: firmware enables internal pull-ups and applies debounce.

```text
Start:   Pico GP6 / physical 9  ---[ normally-open button ]--- Pico GND
Select:  Pico GP7 / physical 10 ---[ normally-open button ]--- Pico GND
```

| Optional direction | Pico GPIO / physical pin | Other button contact |
|---|---|---|
| Up | GP8 / 11 | Pico GND |
| Down | GP9 / 12 | Pico GND |
| Left | GP10 / 14 | Pico GND |
| Right | GP11 / 15 | Pico GND |

A four-leg tactile switch often has two pairs that are already internally connected. Use continuity mode **unpowered** to choose two contacts that are open when released and connected when pressed. Mount it so breadboard rows do not permanently join those contacts. Never connect a button between 5V and GND or use DIN pin 2 as its ground.

If the row-24 ground node fills up, extend it with a jumper from a free hole (for example e24) to a new unused left-side row, such as a26; that row becomes another Pico GND node. Check it with continuity. Do not squeeze multiple wires into one hole.

## 9. Final Alesis → PS3 connection

```text
[Alesis Nitro Mesh] <--- its own Alesis power adapter
        |
        | MIDI OUT
        v
[5-pin male-to-male MIDI cable]
        |
        v
[DIY DIN MIDI IN socket]
        |
        v
[6N138 isolated receiver]
        |
        | output to GP1 (physical pin 2)
        v
[Raspberry Pi Pico H / RP2040]
        |
        | Micro-USB DATA cable (also carries power TO Pico)
        v
[PlayStation 3 USB-A port]
        |
        v
[Rock Band]
```

Power comes from **two separate places**: the Nitro uses its own adapter; PS3 USB powers Pico, whose VBUS and 3V3 outputs supply the receiver as wired. No breadboard power module or external Pico power supply is needed. The debug adapter never powers the circuit. Keep the board where it cannot be bumped by drumsticks or pedals.

## 10. First PS3 test: standard firmware only

Use **[dist/standard/nitro_ps3.uf2](../dist/standard/nitro_ps3.uf2)**. **Do not start with Pro firmware.** Use a stock PS3: no HEN, CFW, PS3XPAD, authentication donor or USB hub is part of this test. Keep a normal DualShock/Sixaxis available to navigate and launch the game.

1. Flash the standard UF2 using BOOTSEL, then disconnect from the computer.
2. Turn Nitro and PS3 off; disconnect Pico USB. Recheck that the voltage pre-flight passed.
3. Connect Nitro **MIDI OUT** to adapter **MIDI IN** with the MIDI cable.
4. Power Nitro with its own adapter; stop songs/demos/metronome.
5. Connect Pico directly to a PS3 USB port using the Micro-USB data cable.
6. Boot PS3. Pico gets USB power when that port is powered.
7. Use the normal controller to launch Rock Band.
8. Reach the player/instrument join screen. Menu wording differs by game.
9. Press the optional adapter **Start** button if installed. If omitted, use whatever joining/navigation the game permits; inability to join without buttons is not yet proof of a MIDI fault.
10. Observe whether the game offers or joins a **drum slot**. Select standard drums and leave Pro/cymbal options disabled.

**Failure to control the PS3 XMB does NOT mean drum enumeration failed.** XMB is the console's home menu; this adapter has no PS/Home button and is intended for the game's drum role.

The basic **P0 success condition** is: **the stock PS3 and Rock Band recognize the Pico as drums without console modification or another instrument.** This remains an actual hardware test. Do not install PS3XPAD to make this test pass. Record PS3 model/system version and game title/region/update version, especially if recognition fails. Use the [detailed PS3 test plan](ps3-test-plan.md) for a results sheet.

## 11. Test individual inputs

Choose a practice/freestyle area or easy predictable song where you can see drum feedback. Test roughly **20 deliberate hits per input**, including soft, medium and hard strikes. A miss against a song chart can also be playing/calibration timing; use clear feedback and debug logs to distinguish that from an absent input.

| Hit this | Expect this in standard drums |
|---|---|
| Snare, and rim if available | Red |
| Tom 1 | Yellow |
| Tom 2 | Blue |
| Tom 3 | Green |
| Hi-hat, open/closed | Yellow |
| Ride | Blue |
| Crash | Green |
| Kick pedal | Kick |

Write down missed hits, two responses for one hit, wrong colors and stuck inputs. Stop for ten seconds: no lane should remain held. Close the hi-hat pedal without striking a pad; pedal/splash alone should not produce a game strike with the default mapping. Correct individual-pad problems before moving on.

## 12. Test chords: hits at the same time

A chord here means two or more drum inputs together. Repeat each pattern about 20–30 times slowly and watch for every intended lane:

- **Red + yellow + kick:** snare, tom 1 and kick.
- **Blue + green:** tom 2 and tom 3.
- **Crash + kick:** green and kick in standard mode.
- **Hi-hat + snare + kick:** yellow, red and kick.

Record any missing lane. The MIDI cable sends note bytes one after another, but the firmware can overlap their controller states. Consistently missing one source warrants comparing its incoming notes, not assuming simultaneous hits are impossible.

## 13. Test rolls and repeated inputs

Play repeated snare hits for about ten seconds at each achievable rate: **4, 8, 12, 16 hits/second**, then faster if possible. Do not treat inability to play a precise rate as a circuit failure. An external metronome can help; keep the module's own playback off for diagnosis.

Also try alternating snare/hi-hat, repeated kick, and runs across the toms. Watch for merged hits, missed hits, doubles or growing delay. The software's simulated high-rate tests do not guarantee the same rate is accepted by the game.

If drops occur, stop, unplug before changing connections, and reflash **standard-debug** using section 7. Observe actual incoming `on` events and error counters. You can keep Pico USB on PS3 and read GP4 using the separate computer adapter. Do not change firmware timing blindly; first establish whether the missing hit was ever received as MIDI. Record the rate, note, velocity, counters and game behavior, then return to standard after diagnosing.

## 14. Pro Drums only after standard works

Only after standard recognition, individual inputs, chords and rolls pass should you try **[dist/pro/nitro_ps3.uf2](../dist/pro/nitro_ps3.uf2)**. Flash it with the same BOOTSEL procedure and select the appropriate Pro Drums/cymbal settings in **Rock Band 3**.

| Trigger | Expected Pro role |
|---|---|
| Tom 1 | Yellow pad |
| Tom 2 | Blue pad |
| Tom 3 | Green pad |
| Hi-hat | Yellow cymbal |
| Ride | Blue cymbal |
| Crash | Green cymbal |
| Snare | Red |
| Kick | Kick |

**Pro remains experimental.** The protocol uses shared flags, D-pad and velocity fields. Some combinations, including red with a same-color pad+cymbal pair, have known limitations; see [protocol.md](protocol.md). Test pad/cymbal differences separately before complex combinations. Return to the standard UF2 if Pro causes problems; a Pro-specific failure does not invalidate a recorded standard-mode pass.

## 15. Troubleshooting for beginners

Disconnect power before moving wires. Change one thing at a time and write down what happened.

| Symptom | What to check next |
|---|---|
| Pico appears not to power | A dark LED alone is inconclusive. Check the data cable, USB connection/PS3 port and VBUS with the meter. If the board gets hot or wiring may be shorted, unplug immediately and inspect unpowered. |
| RPI-RP2 never appears | Disconnect, hold BOOTSEL **before** connecting, release after connection. Try a known data cable and another computer USB port. Test the Pico alone with all circuit wires disconnected. |
| Firmware copies but computer sees no controller | Reconnect without BOOTSEL; confirm the file came from `dist/standard`, not an HTML download or ZIP. Check VID/PID in section 3 and try a different data cable. |
| No serial text at all | Confirm **debug** firmware, correct USB-TTL port, adapter powered through its own USB, GP4→RX and GND→GND, 115200 8-N-1/no flow control. Pico's own USB is not the serial port. |
| Statistics appear but no MIDI events | Check Nitro **MIDI OUT**, cable, actual DIN 4/5 identities and mirrored views, 6N138 notch orientation, approximately 5V at pin 8, GND at pin 5, GP1 idle about 3.3V, and diode stripe toward pin 2. Measure only with the prescribed method; unplug before fixing. |
| GP1 measures near 5V | **Disconnect immediately.** Check that the 1kΩ resistor starts at Pico pin 36/3V3, not pin 40/VBUS; check for bridged rows. Do not proceed to PS3. |
| Garbled serial text | Check 115200 baud and adapter signal-level documentation; ensure a common **Pico-side** ground. Do not add DIN/Nitro ground. |
| UART errors or RX overflow | Recheck receiver wiring and component values. A scope may be needed for slow/malformed edges; a static 3.3V reading cannot confirm timing. |
| MIDI logs work but PS3 does not recognize drums | Reflash **standard**, remove hubs, try another PS3 USB port/data cable, cold boot with Pico attached, then separately try attaching after boot before game launch. Verify PC VID 12BA/PID 0210. Record PS3 model/firmware, game title/region/update version. Follow the test plan; **do not install PS3XPAD as a workaround**. |
| Wrong pad/color or one pad absent | Log the actual note and compare it with [config.h](../include/config.h). A customized Nitro kit may send another note. Firmware remapping requires a rebuild; restore/review module settings using its manual if appropriate. |
| Double hits | Check for two `on` lines from one strike. Head/rim triggering, vibrations reaching another pad (cross-talk), or module retrigger settings can cause extra events. Adjust Nitro sensitivity/threshold/cross-talk/retrigger settings carefully, one setting at a time. An `off` line is not an extra strike. |
| Rolls drop or lag | Check incoming-note count and counters, use standard mode, note the playing rate. Backlogged queues and game sampling require different fixes; provide logs before changing pulse timing. |
| Start always held or does nothing | With power off, test the button's chosen contacts: open released, closed pressed. Check GP6 is **physical 9** and the other contact is Pico GND; four-leg switches can be wired across an already joined pair. |
| Only Pro combinations fail | Reflash standard and read the Pro shared-field limitations before assuming a wiring fault. |

## 16. Complete checklist

- [ ] Correct original **RP2040 Pico H** purchased, not Pico 2.
- [ ] Standard UF2 downloaded as a binary file and flashed.
- [ ] USB device enumerates on the PC with VID 12BA / PID 0210.
- [ ] Breadboard connected groups understood and checked.
- [ ] 6N138 orientation correct, straddling the center trench.
- [ ] DIN pins identified from the actual connector, not a guessed mirrored view.
- [ ] 220Ω, 1kΩ, 4.7kΩ and 100nF parts verified.
- [ ] Diode stripe goes toward 6N138 pin 2.
- [ ] 5V supply correct at 6N138 pin 8.
- [ ] 3.3V supply correct at Pico physical 36.
- [ ] GP1 idle about 3.3V, never pulled up to 5V.
- [ ] No MIDI signal, DIN pin 2/shield, or Nitro ground connected directly to Pico GND.
- [ ] Nitro MIDI OUT → adapter MIDI IN.
- [ ] Debug MIDI seen for every applicable pad (or explicitly mark this optional diagnostic as not performed).
- [ ] UART/error counters zero under testing; any host-dependent queue overflow investigated.
- [ ] Standard UF2 reflashed after debugging.
- [ ] Direct Pico USB connection to stock PS3, no hub or console modification.
- [ ] Drum slot recognized.
- [ ] Individual inputs pass, including soft/medium/hard hits.
- [ ] Chords pass.
- [ ] Rolls pass.
- [ ] Pro only tested afterward, if desired; experimental limits recorded.

Check a box only for something actually verified. Keep the firmware filename/folder, game version and results with a photo of the circuit. Use the [PS3 test plan](ps3-test-plan.md) for longer sessions, reconnect checks and tests across Rock Band titles.

## 17. After the breadboard works

**Do not solder a permanent version until the breadboard prototype works through the standard PS3 tests.** A neatly soldered circuit is harder to correct if its wiring was never proven.

Photograph the working breadboard from above and from angles that show DIN labels, the optocoupler notch, diode stripe and Pico wires. Save a connection list and the exact working UF2. Label every wire before removing anything.

A later permanent build can use **perfboard** (a board of solderable holes), a **custom PCB**, a panel-mount DIN connector, Start/Select buttons and an enclosure with cable strain relief. Ask an experienced builder for help with first-time soldering. Preserve the same isolation boundary and the separate 5V supply/3.3V output pull-up. After moving to a permanent board, repeat the unpowered checks, voltage pre-flight and gameplay tests; successful breadboard results do not automatically verify new assembly work.
