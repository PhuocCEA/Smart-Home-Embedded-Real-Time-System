
## 📌 Core Functionalities

The system processes and acts upon four main concurrent real-time application sub-tasks without the overhead of a full RTOS kernel:

1. **💡 Automated Light Control:** Monitors ambient light intensity using a Light Dependent Resistor (LDR) interfaced via a 10-bit successive-approximation ADC (Channel `AN0`). It automates high-voltage AC/DC lighting loads via a 12V SPDT Relay array driven by a **ULN2003A** Darlington transistor array based on an adjustable software threshold (Default: `< 500` counts).
2. **🌡️ Climate Monitoring:** Samples atmospheric temperature and relative humidity from a **DHT11** sensor using a custom bit-banged single-wire digital interface.
3. **🔍 Motion Detection & Intrusion Alarm:** Implements an active security loop utilizing a Passive Infrared (**PIR**) motion sensor. Intrusions trigger an immediate local hardware alert via a dedicated active piezoceramic buzzer and indicator LED on pin `RD1` with a deadline constraint of $< 100$ ms.
4. **📺 Multi-Page Multiplexed LCD Display:** Drives a 16x2 character Text LCD interface divided into three distinct operational data pages. Pages rotate sequentially every 2 seconds (complete 6-second cycle) to continuously report peripheral metrics without dropping real-time sensor tasks.

---

## 🛠️ Hardware Design & PCB Architecture

The hardware is consolidated on a structurally optimized, custom-etched Printed Circuit Board (PCB) engineered using **Altium Designer** to achieve low electromagnetic interference (EMI) and robust signal integrity.

### 1. Power Supply Chain
Designed to accept external wide-input DC power, convert it with minimal ripple, and protect sensitive digital microcontrollers against electrical transients.
```
[DC Barrel Jack (JDC1)] ──> [Reverse Polarity Protection Diode] ──> [1000 µF / 16V Bulk Electrolytic Filter Cap] ──> [LM7805 Linear Regulator (5V Stable Output)] ──> [100 nF + 10 µF Decoupling Capacitors] ──> [System Power Rails & Power LED]
```

### 2. Physical Layout Strategy
* **Sensor Input Zone (Left Side):** Isolated headers for input nodes (`GAS`, `LIGHT/LDR`, `DHT11`, `PIR`) mapped away from inductive loads.
* **MCU Central Core:** Houses the PIC16F877A IC package, decoupling capacitors placed flush against VDD pins, and an 8 MHz crystal oscillator utilizing short, symmetric PCB traces to minimize parasitic capacitance.
* **Relay Actuation Grid (Bottom Section):** Houses four 10A/12V DC electromechanical relays isolated alongside high-current tracks. Flyback freewheeling diodes are placed across relay coils to clamp inductive voltage spikes.
* **IoT Expansion Node (Right Side):** Pre-routed PCB footprints for the ESP32 DevKit micro-controller board and an I2C-driven **SSD1306 OLED** screen.
* **In-Circuit Serial Programming (ICSP):** Dedicated header breakout for direct firmware flashes via a PICKit programmer without removing the MCU.

---

## 💾 Firmware Architecture & Task Scheduling

The software stack is developed entirely in **Bare-Metal C** optimized for the **Microchip XC8 Compiler** inside the **MPLAB X IDE**. 

### 1. Modular Hardware Abstraction Layer (HAL)
The codebase avoids monolithic code smell through rigid encapsulation:
* `LightSensor.h/.c`: Coordinates peripheral ADC initialization, 10-bit channel selection, software hysteretic tracking, and macro-driven relay toggling.
* `DHT11.h/.c`: Implements microsecond-precise single-wire protocol handling, handling strict handshake timings, reading the 40-bit data packet, and validating data with an algebraic checksum verification algorithm.
* `PIR.h/.c`: Executes logic processing on digital input pins for low-latency security detection.

### 2. Non-Blocking Tick-Based Cooperative Scheduler
To fulfill the multi-tasking demands of a real-time system on an 8-bit architecture, the software completely deprecates blocking delay loops (e.g., `__delay_ms()`). A hardware timer (Timer0/Timer1) generates a rigid baseline system tick every **100 ms**. Tasks are scheduled dynamically across a macro-cycle of 60 ticks (6 seconds):

| Task Routine | Execution Period | Functional Behavior |
| :--- | :--- | :--- |
| **Relay Regulation** | Every 100 ms (Every Tick) | Samples `AN0` ADC $
ightarrow$ compares against threshold 500 $
ightarrow$ updates Relay array status. |
| **Security Monitor** | Every 100 ms (Every Tick) | Polls PIR GPIO state $
ightarrow$ Instantly commands `RD1` buzzer if high-priority trigger occurs. |
| **LCD Frame 0** | Ticks 0 to 19 (0s – 2s) | Updates Screen: Prints raw LDR ADC integer counts and active Relay states. |
| **LCD Frame 1** | Ticks 20 to 39 (2s – 4s) | Decodes single-wire stream: Fetches and displays Temperature (°C) & Humidity (%). |
| **LCD Frame 2** | Ticks 40 to 59 (4s – 6s) | Updates Screen: Reports security diagnostic logs (`MOTION DETECT` or `SYSTEM CLEAR`). |

---

## 📊 Experimental Results

The physical prototype has been fully verified and benchmarked under true operational environments:
* **Ambient Tracking:** In fully lit conditions, the LDR outputs ADC integer values ranging between `820 – 1000` (Relay opens/turns off). Obscuring the sensor reduces values to `100 – 300`, causing the relay to latch closed instantly.
* **Climatic Precision:** Environmental data reliably resolves within normal room boundaries (28.0°C – 30.0°C; 65% – 75% RH) matching calibrated master equipment.
* **Real-Time Deadlines:** Intrusion response latency (PIR stimulus to Buzzer resonance) remains strictly bounded under **100 ms**, fulfilling critical safety parameters.

---

## ⚠️ Known Limitations & Firmware Roadmap

### Identified Bottlenecks:
1. **DHT11 Timing Sensitivity:** The single-wire protocol requires precise microsecond pulse width measurements. Legacy synchronous LCD write operations can introduce jitter during time-critical sampling windows.
2. **ADC Threshold Jitter:** Minor power supply ripple caused by electromechanical relay latching can trigger boundary oscillations (jitter) near the 500 count threshold.

### Engineering Roadmap & Mitigations:
* 📉 **Analog Conditioning:** Integrate an external hardware low-pass RC filter ($10	ext{ k}\Omega + 100	ext{ nF}$) on pin `AN0` to eliminate high-frequency input ripple.
* ⚙️ **Asynchronous Driver Refactoring:** Rewrite the LCD driver library into a **Non-Blocking State Machine**, completely offloading data transfers from the main execution loop into an Interrupt Service Routine (ISR).
* 📡 **IoT Integration Phase:** Establish high-speed full-duplex **UART** communication between the PIC16F877A and the onboard **ESP32**. The ESP32 will run **FreeRTOS** to independently upload telemetry streams to an **MQTT Broker** for remote visualization on a **Node-RED Cloud Dashboard** and update local graphics on the **SSD1306 OLED** screen.
