# Step-by-Step Wiring & Hardware Assembly Guide

This document provides comprehensive, terminal-by-terminal wiring instructions for building the **Arduino Nano Line Follower Robot** based strictly on the hardware wiring reference.

---

## 1. Power Distribution Assembly

### 1.1 Battery Connector & Protection
1. **XT60 Battery Lead**:
   - Solder a high-current XT60 male/female pigtail according to your battery pack.
   - Solder an inline automotive mini-blade fuse holder directly onto the **Positive (RED)** wire as close to the XT60 connector as physically possible.
   - Insert an appropriately rated blade fuse (e.g., 3A to 5A, calculated from motor stall current + 1A margin).
2. **Double-Pole Double-Throw (DPDT) Main Switch**:
   - The DPDT switch isolates **BOTH** the positive power rail and the ground rail simultaneously when switched OFF.
   - **Pole 1 (Positive Rail)**:
     - Connect the fused positive wire from the battery to **Pole 1 Input (Center pin or Common 1)**.
     - Connect the **Pole 1 Output (NO contact 1)** to the **Switched Positive Rail**.
   - **Pole 2 (Ground Rail)**:
     - Connect the battery negative (BLACK) wire to **Pole 2 Input (Center pin or Common 2)**.
     - Connect the **Pole 2 Output (NO contact 2)** to the **Switched Ground (GND) Rail**.
   - *Verification*: Use a digital multimeter in continuity mode to verify the switch terminals before soldering. Many switches have cross or offset internal contact mechanisms.

### 1.2 Buck Converter Calibration & Wiring
1. **Do NOT connect the Arduino Nano or LCD yet.**
2. Connect **Buck `IN+`** to the **Switched Positive Rail**.
3. Connect **Buck `IN-`** to the **Switched GND Rail**.
4. Power on the system via the DPDT switch.
5. Place your digital multimeter probes across **Buck `OUT+`** and **Buck `OUT-`**.
6. Adjust the multi-turn potentiometer on the buck converter until your multimeter displays exactly **5.00V**.
7. Power OFF the system.
8. Wire **Buck `OUT+`** to:
   - Arduino Nano **5V pin** (*NEVER connect to VIN, as VIN requires at least 6.5V - 7V to pass the internal low-dropout regulator*).
   - TB6612FNG **VCC pin**.
   - 16x2 I2C LCD **VCC pin**.
9. Wire **Buck `OUT-`** to:
   - Arduino Nano **GND pin**.
   - TB6612FNG **GND pin**.
   - 16x2 I2C LCD **GND pin**.
   - Switched GND rail (ensuring a common ground reference across all modules).

---

## 2. TB6612FNG Dual H-Bridge Motor Driver Wiring

| TB6612 Pin | Connected To | Wire Color / Notes |
| :--- | :--- | :--- |
| **VM** | Switched Positive Rail | Unregulated battery voltage (direct from DPDT Pole 1 Output) |
| **VCC** | Buck `OUT+` (5.0V) | Regulated logic voltage |
| **GND** | Switched GND Rail | Common logic & power ground |
| **STBY** | Arduino Nano **D4** | Active HIGH standby enable |
| **PWMA** | Arduino Nano **D5** | Hardware PWM (Timer0, 980Hz) |
| **AIN1** | Arduino Nano **D7** | Left motor logic input 1 |
| **AIN2** | Arduino Nano **D8** | Left motor logic input 2 |
| **AO1** | Left Motor Terminal 1 | Left motor H-bridge output |
| **AO2** | Left Motor Terminal 2 | Left motor H-bridge output |
| **PWMB** | Arduino Nano **D6** | Hardware PWM (Timer0, 980Hz) |
| **BIN1** | Arduino Nano **D9** | Right motor logic input 1 |
| **BIN2** | Arduino Nano **D10** | Right motor logic input 2 |
| **BO1** | Right Motor Terminal 1 | Right motor H-bridge output |
| **BO2** | Right Motor Terminal 2 | Right motor H-bridge output |

### Motor Logic Table
| IN1 | IN2 | PWM | Mode / Direction |
| :---: | :---: | :---: | :--- |
| H | L | PWM | Forward |
| L | H | PWM | Reverse |
| L | L | X | Coast / Stop (High impedance) |
| H | H | X | Short Brake (Dynamic braking) |

*If Left or Right motor rotates backward during testing, swap `AO1`/`AO2` or `BO1`/`BO2`, or invert `AIN1`/`AIN2` or `BIN1`/`BIN2` in software.*

---

## 3. 16x2 I2C LCD Backpack Wiring

| LCD Pin | Connected To | Wire Color / Notes |
| :--- | :--- | :--- |
| **GND** | Arduino Nano `GND` | Ground reference |
| **VCC** | Regulated 5.0V (Buck `OUT+` / Nano 5V) | Logic & backlight power |
| **SDA** | Arduino Nano **A4** (SDA) | I2C Serial Data bus |
| **SCL** | Arduino Nano **A5** (SCL) | I2C Serial Clock bus |

- Adjust the blue potentiometer on the rear of the I2C backpack using a small screwdriver to set the display text contrast.

---

## 4. Four Menu Push Buttons Wiring

Four normally-open momentary tactile buttons are wired directly between Arduino Nano digital input pins and **GND**:

```
[Nano Pin D2]  -----(Button UP)-----  [Common GND]
[Nano Pin D3]  -----(Button DOWN)---  [Common GND]
[Nano Pin D11] -----(Button ENTER)--  [Common GND]
[Nano Pin D12] -----(Button BACK)---  [Common GND]
```

- Each button utilizes the ATmega328P internal pull-up resistor via `pinMode(pin, INPUT_PULLUP)`.
- When the button is **open (idle)**, digital read returns `HIGH`.
- When the button is **pressed**, it closes to GND and digital read returns `LOW`.

---

## 5. Line Sensor Array Header (A0–A3, A6, A7)

The analog and auxiliary pins are reserved for sensor inputs:

| Pin | Reserved Function |
| :--- | :--- |
| **A0** | Line Sensor Channel 1 |
| **A1** | Line Sensor Channel 2 |
| **A2** | Line Sensor Channel 3 |
| **A3** | Line Sensor Channel 4 |
| **A6** | Line Sensor Channel 5 (Analog input only) |
| **A7** | Line Sensor Channel 6 / Battery Monitor (Analog input only) |

> **Grounding Rule**: Ensure the line sensor ground pin connects directly to the system switched GND rail.
