# 🚉 Smart Railway Platform Clock & Announcement Controller

An embedded systems project built on the **NXP LPC2148 (ARM7TDMI-S)** microcontroller that turns a single 16x2 LCD into an automated railway platform display — alternating between a live clock/date view and a three-window train-announcement view (all-trains cycle → upcoming train → active train), backed by LED/buzzer status alerts and a keypad-driven, password-protected Admin Mode for on-site schedule editing.

---

## 📑 Table of Contents

1. [Project Overview / Purpose](#-project-overview--purpose)
2. [Features and Functionality](#-features-and-functionality)
3. [Repository Structure](#-repository-structure)
4. [System Architecture (Block Diagram)](#-system-architecture-block-diagram)
5. [Flow Chart](#-flow-chart)
6. [Hardware Details](#-hardware-details)
7. [Pin Mapping](#-pin-mapping)
8. [Software Setup & Build Instructions](#-software-setup--build-instructions)
9. [Admin Mode — Usage Guide](#-admin-mode--usage-guide)
10. [Testing Instructions](#-testing-instructions)
11. [Troubleshooting Guidelines](#-troubleshooting-guidelines)
12. [Real-Time Use Cases](#-real-time-use-cases)
13. [Application Screenshots / Results](#-application-screenshots--results)
14. [Project Submission Checklist](#-project-submission-checklist)
15. [Author](#-author)

---

## 🎯 Project Overview / Purpose

### Objective
At most small and mid-sized railway stations, platform information (arrival/departure times, delays, platform numbers) is either updated manually on physical boards or not updated in real time at all, leading to passenger confusion and missed trains. This project implements a **low-cost, standalone embedded controller** that automatically manages a railway platform's clock and train-announcement display without depending on a central server, network connection, or manual paperwork.

### Business Problem Addressed
- Manual platform boards are slow to update and prone to human error.
- Passengers have no immediate visual/audio cue when a train is approaching or delayed.
- Station staff need a simple, on-the-spot way to update schedule data (e.g., after a delay is reported) without a PC or specialized software.

### Scope
The system is a **single-station, single-platform prototype** that:
- Maintains the current date/time using the LPC2148's **on-chip RTC** (independent of any external time source).
- Stores and manages a small table of trains (`TOTAL_TRAINS = 6` in the current build, in `trainDB.c` — easily extendable).
- Automatically evaluates each train's status (**On-Time / Arrived / Departed**) every cycle and reflects the platform's worst-case state through 3 status LEDs and a buzzer.
- Cycles the single 16x2 LCD through a **3-window train display** — all trains in rotation, then a strict lock onto the next upcoming train 5 minutes before arrival, then a strict lock onto the currently active train from arrival until 30 seconds after departure.
- Provides a keypad-based, **PIN-protected Admin Mode** (fired by a hardware interrupt) so station staff can update train timing, platform number, and delay minutes, or correct the system date/time — all without a PC connection.
- Is designed to be reproducible on standard LPC2148 prototyping hardware, making it suitable as an academic embedded-systems demonstration as well as a proof-of-concept for low-cost station automation.

**Out of scope (by design, for this prototype):** multi-platform support, wireless schedule updates, integration with live railway APIs, and voice-based announcements (a buzzer + LCD scroll is used as a stand-in for a public address system).

---

## ✨ Features and Functionality

| Feature | Description |
|---|---|
| **Live Clock & Calendar Display** | Every ~12 seconds of the display cycle, the LCD shows the current time (HH:MM:SS), day of week, and date (DD/MM/YYYY), sourced from the LPC2148's on-chip RTC. |
| **Automatic Train Status Evaluation** | Every loop iteration, `CheckTrainSchedules()` compares the RTC time against each train's (delay-adjusted) arrival/departure time and assigns a status: **On-Time**, **Arrived**, or **Departed**. |
| **3-Level Status Indication (LEDs)** | 🟢 Green = On-Time, 🟡 Yellow = a train has Arrived (approaching/at platform), 🔴 Red = a train is running with delay minutes. |
| **3-Window Train Announcement Display** | **Window 1** cycles through all trains; **Window 2** locks onto the next upcoming train starting 5 minutes before its arrival; **Window 3** locks onto the currently active train from arrival through 30 seconds after departure. |
| **Scrolling Train Name & Destination** | Line 1 of the LCD scrolls the train name and destination horizontally while Line 2 alternates between column headers and live values. |
| **Buzzer Alert** | The buzzer pulses whenever a train is currently at the platform (Arrived state), audibly flagging its presence. |
| **Admin Edit Mode (Interrupt-Driven)** | Pressing a dedicated Admin switch fires an **external hardware interrupt (EINT1)**, setting a flag that the main loop checks — no polling delay, no reboot or reflash required. |
| **PIN-Protected Access** | Admin Mode requires a 4-digit PIN (default `1234`, changeable from within the menu) with a maximum of 3 attempts before the menu locks out. |
| **Keypad-Based Data Entry** | A 4x4 matrix keypad (`1-9,0,A,B,C,D,*,#`) is used to type numeric values, with live LCD echo, `C` as backspace, and `D` to clear/cancel the current entry. |
| **Editable Train Records** | Per train: platform number, delay (minutes, which auto-recomputes arrival/departure), and direct arrival/departure hour & minute can all be updated from the Admin Menu. |
| **Editable System Clock** | Admin can correct the RTC's hour, minute, date, month, year, and day-of-week directly from the keypad, in strict 24-hour railway format. |
| **Changeable Admin PIN** | The 4-digit admin PIN can be updated from within the dashboard (with confirmation match check). |
| **Non-Volatile-Style Timekeeping** | The RTC is on-chip and keeps running independently of the main loop once initialized. |
| **Serial Programming Interface** | A DB9 (UART) connector is used with Flash Magic to program the board during development. |

---

## 📁 Repository Structure

```
Smart-Railway-Platform-Clock-Announcement-Controller/
│
├── application.c            # main() — init, admin-flag check, RTC read, status eval, dual-mode LCD loop
├── scheduler.h / scheduler.c  # CheckTrainSchedules() + 3-window train display logic (DisplayActiveTrainStatus)
├── trainDB.h / trainDB.c      # TrainInfo_t structure + the 6-train schedule database
├── menu.h / menu.c            # Password check, RTC edit menus, train-record edit menu, Admin Dashboard
├── rtc.h / rtc.c               # On-chip RTC init, get/set time, date, and day-of-week
├── lcd.h / lcd.c               # 8-bit LCD driver — init, command/char/string/number writes, CGRAM
├── kpm.h / kpm.c               # 4x4 keypad scanning, KeyScan(), ReadNum(), ReadPass()
├── led_buzz.h / led_buzz.c     # Status LED (On-Time/Approach/Delayed) and buzzer control
├── eint1_sw.h / eint1_sw.c     # EINT1 admin-switch interrupt init & ISR (sets admin_flag)
├── delay.h / delay.c           # us/ms/s software delay routines
├── all_peripheral_defines.h    # Centralized pin map, bit-macros, clock/RTC prescaler constants
├── types.h                     # Project-wide typedefs (u8, s8, u16, u32, f32, ...)
├── Startup.s                   # ARM7 startup/vector table assembly file
├── *.uvproj / *.sct            # Keil µVision project & scatter-load files
└── images/                     # Block diagram and flow chart (this README)
    ├── block_diagram.png
    └── flowchart.png
```

### Key Files at a Glance
| File | Why it matters |
|---|---|
| `application.c` | Entry point — controls the overall loop: admin-flag check → RTC read → schedule check → LED/buzzer drive → alternating LCD display. |
| `scheduler.c` | The "brain" of the announcement system — `CheckTrainSchedules()` sets each train's status, `DisplayActiveTrainStatus()` implements the 3-window LCD logic. |
| `trainDB.h` / `trainDB.c` | Change `TOTAL_TRAINS` in `trainDB.h` and add/edit entries in `trainDB.c` to scale or re-purpose the schedule. |
| `menu.c` | All admin-facing data entry logic (PIN check, time/date/day edit, per-train edit, PIN change) lives here. |
| `rtc.c` | Low-level on-chip RTC configuration — the single source of truth for all timing. |
| `all_peripheral_defines.h` | One place to look up or change every pin assignment and LCD/RTC constant. |

---

## 🧩 System Architecture (Block Diagram)

<img width="1264" height="842" alt="block_diagram (1)" src="https://github.com/user-attachments/assets/8e82e4ec-13c0-4285-a235-be1742fd4b06" />

**Signal flow:**
- **Inputs** → Admin Edit Switch (external interrupt, EINT1), 4x4 Matrix Keypad, the LPC2148's on-chip RTC, and the train schedule table stored in MCU memory (`trainDB.c`).
- **Processing (LPC2148)** → reads the RTC, compares it against the schedule (`CheckTrainSchedules`), updates the LCD, detects delays/arrivals, drives the LED/buzzer indicators, and handles admin configuration (`RunAdminDashboard`).
- **Outputs** → a 16x2 character LCD (alternating Clock/Date view and Train Announcement view), 3 status LEDs (Green/Yellow/Red), and a buzzer.
- A DB9 serial connector is used to program/communicate with the board from a PC during development.

> **Note:** the block diagram above illustrates the design conceptually as two LCD "roles" (clock/date and train information). In the current build both roles are served by a **single physical 16x2 LCD** that alternates between the two views on a timer (`lcdMode` in `application.c`), keeping the hardware bill-of-materials to one LCD, one keypad, and one switch.

---

## 🔄 Flow Chart

End-to-end process flow of the firmware's main loop and the 3-window train display logic:

<img width="1024" height="1536" alt="flowchart" src="https://github.com/user-attachments/assets/fce73b44-c216-476f-8460-160adb0bc820" />


**Data movement summary:**
1. RTC (`GetRTCTimeInfo` / `GetRTCDateInfo` / `GetRTCDay`) → `CheckTrainSchedules()` compares current time vs. each train's delay-adjusted arrival/departure time.
2. Train DB (`TrainDB[]`) → status logic (`STATUS_ON_TIME` / `STATUS_ARRIVED` / `STATUS_DEPARTED`) and the LCD announcement renderer.
3. Status result → LED driver (`LED_OnTime_Ctrl`, `LED_Approach_Ctrl`, `LED_Delayed_Ctrl`) and buzzer pulse (`Buzzer_Ctrl`) whenever any train is currently arrived.
4. Admin switch interrupt (EINT1, P0.3) → `admin_flag` → the main loop breaks into `RunAdminDashboard()`, which (after PIN check) writes back into the RTC and `TrainDB[]`.
5. Inside the Train Announcement view, `DisplayActiveTrainStatus()` decides the active window every cycle:
   - **Window 1 — All Trains:** rotates through every train every ~30 seconds (no train within 5 minutes of arrival, none currently on platform).
   - **Window 2 — Upcoming Train:** locks onto the specific train whose arrival is within the next 5 minutes.
   - **Window 3 — Active Train:** locks onto the train from its arrival time until 30 seconds after its departure time, cycling through values → column headers → an `ARRIVED` timestamp frame, then holding a `DEPARTED` frame once departure time has passed.

---

## 🔧 Hardware Details

| Component | Quantity | Role in the System | Notes |
|---|---|---|---|
| LPC2148 (ARM7TDMI-S) development board | 1 | Core controller — runs all firmware logic | `F_OSC = 12 MHz`, `CCLK = 5 × F_OSC = 60 MHz`, `PCLK = CCLK / 4` (see `all_peripheral_defines.h`) |
| 16x2 Character LCD | 1 | Alternates between Clock/Date view and Train Announcement view | 8-bit parallel interface |
| 4x4 Matrix Keypad | 1 | Admin data entry — digits `0–9`, and `A / B / C / D / * / #` as menu & editing controls | Connected to Port 1 |
| Push-Button Switch | 1 | Triggers Admin Mode via external interrupt (EINT1) | Connected to P0.3 |
| LEDs — Green, Yellow, Red | 3 | Visual train-status indication (On-Time / Arrived / Delayed) | P1.27 / P1.28 / P1.29 |
| Active Buzzer | 1 | Audible alert while a train is at the platform | P1.26 |
| DB9 Serial Connector | 1 | UART link for PC ↔ MCU programming/debugging | Used with Flash Magic or similar |
| Regulated 5V DC Power Supply | 1 | Powers the board and peripherals | |

> 📷 **Add photographs of your assembled hardware here** — e.g., `images/hardware/full_setup.jpg`, `images/hardware/lcd_display.jpg`, `images/hardware/keypad_wiring.jpg` — with a short caption for each describing what is shown (top view, wiring close-up, powered-on demo, etc.).

---

## 🔌 Pin Mapping

Sourced directly from `all_peripheral_defines.h`:

| Peripheral | Function | MCU Pin |
|---|---|---|
| Admin Edit Switch | External Interrupt (EINT1) | P0.3 |
| Buzzer | Delay/arrival alert | P1.26 |
| Green LED | On-Time status | P1.27 |
| Yellow LED | Approaching / Arrived status | P1.28 |
| Red LED | Delayed status | P1.29 |
| LCD Data Bus (D0–D7) | 8-bit data | P0.6 – P0.13 |
| LCD RS | Register Select | P0.17 |
| LCD EN | Enable | P0.18 |
| LCD RW | Read/Write | P0.19 |
| Keypad Rows (R0–R3) | Row scan | P1.16 – P1.19 |
| Keypad Columns (C0–C3) | Column scan | P1.20 – P1.23 |

---

## 🛠️ Software Setup & Build Instructions

**Tools required:**
- **MCU:** NXP LPC2148 (ARM7TDMI-S)
- **IDE / Toolchain:** Keil µVision (ARM) — project files (`.uvproj`, `.sct`) are already included
- **Flash Utility:** Flash Magic (via UART/DB9 serial connector)
- **Language:** Embedded C

**Steps:**
1. Clone this repository.
2. Open `Smart Railway Platform Controller.uvproj` in Keil µVision (all `.c`/`.h` files are already part of the project).
3. Build the project (`Project → Build Target`) to generate the `.hex` file.
4. Connect the board to the PC via the DB9 serial connector, and put the board into ISP/bootloader mode as per your board's instructions.
5. Use Flash Magic (or an equivalent LPC21xx flashing tool) to flash the generated `.hex` file onto the LPC2148.
6. Power the board with a regulated 5V DC supply.
7. On power-up, the LCD shows a short "Welcome to Indian Railways" splash, then begins alternating between the Clock/Date view and the Train Announcement view automatically.

---

## 👨‍💼 Admin Mode — Usage Guide

1. Press the **Admin Edit Switch** (P0.3) at any time — this fires **EINT1** and sets `admin_flag`, which the main loop checks and immediately opens `RunAdminDashboard()` (interrupting whatever was on-screen).
2. Enter the **4-digit PIN** (default: `1234`) on the keypad. You get **3 attempts**; a failed attempt shows "Invalid Password" and a 1-second wait before retrying. Pressing `C` or `D` at the PIN prompt exits Admin Mode without changes.
3. From the Admin Dashboard, choose an option:
   - **`A`** → Edit RTC — sub-menu for **Time** (Hour/Minute, strict 24-hour), **Date** (Date/Month/Year), and **Day** (0=Sun … 6=Sat).
   - **`B`** → Edit Train Details — enter a train number to look it up, then edit **Platform** (`1`), **Delay Minutes** (`2`, which auto-recalculates the delay-adjusted arrival/departure shown on the LCD), **Arrival Time** (`3`), or **Departure Time** (`4`). Press `#` to save and exit the record.
   - **`C`** → Edit Admin Password — enter and confirm a new 4-digit PIN.
   - **`D`** → Exit — returns to normal Clock/Train display.
4. While typing any numeric field:
   - Digits **0–9** are echoed live on the LCD (PIN entry echoes `*`).
   - **`C`** acts as backspace.
   - **`D`** clears the field and restarts entry, or cancels/exits the current menu depending on context.
   - **`*`** or **`#`** (or any non-digit key) confirms the value entered and moves on.
5. On a successful update, the system displays a confirmation (e.g., `Hour Updated`, `Plat Updated`, `Train Updated!`, `Password Updated`) and returns to the relevant menu; on exit, it automatically resumes the normal clock/announcement display.

---

## 🧪 Testing Instructions

### Test Setup
1. Flash the firmware as described in [Software Setup & Build Instructions](#-software-setup--build-instructions).
2. Power the board and confirm the LCD initializes correctly (backlight on, splash screen shown, no garbled characters).
3. Compare the RTC's default time/date against a known reference (phone/PC clock) so results are verifiable, and correct it via Admin Mode (`A`) if needed.

### Test Case Matrix

| # | Test Case | Steps | Expected Result |
|---|---|---|---|
| 1 | Power-on | Power the board for the first time | "Welcome to Indian Railways" splash for ~2s, then Clock/Date view appears |
| 2 | Live clock accuracy | Let the system run for 5–10 minutes; compare LCD time against a reference clock | Displayed time matches the reference within a few seconds; date and day are correct |
| 3 | On-time status | Ensure a train's arrival time is well in the future, delay = 0 | Green LED reflects On-Time; that train shows correctly in Window 1 rotation |
| 4 | Upcoming train (Window 2) | Use Admin Mode to set a train's arrival within ~5 minutes of the current RTC time | LCD locks onto that train, showing its number/name/destination and PF/AT/DT/DLY values |
| 5 | Active train (Window 3) | Let the RTC reach a train's arrival time | LCD locks onto that train; Yellow LED turns on; buzzer pulses periodically while it remains "arrived" |
| 6 | Delayed status | Use Admin Menu (`B` → `2`) to set a train's delay minutes > 0 | Red LED turns on; the delay-adjusted arrival/departure and `DLY` value update on the LCD |
| 7 | Departed state | Let the RTC pass a train's (delay-adjusted) departure time | LCD shows a `DEPARTED` frame with the current time for ~30 seconds, then returns to Window 1 |
| 8 | Admin switch interrupt | Press the Admin Edit Switch while the clock or train view is displayed | Display immediately halts and the PIN prompt appears, regardless of what was on-screen |
| 9 | PIN entry — correct | Enter the correct 4-digit PIN | "Valid Password" shown, then the Admin Dashboard (`A/B/C/D`) appears |
| 10 | PIN entry — incorrect | Enter a wrong PIN 3 times | "Invalid Password" and attempt count shown each time; after 3 failed attempts, exits back to normal display |
| 11 | Edit time/date (`A`) | From the dashboard choose `A`, then Time/Date/Day, enter new values | RTC updates immediately; confirmation message shown; Clock view reflects the new values afterward |
| 12 | Edit train record (`B`) | Choose `B`, enter a valid train number, update platform/delay/arrival/departure | Confirmation shown per field; subsequent status evaluation and LCD reflect the new values |
| 13 | Invalid train number | In `B`, enter a train number not present in `TrainDB[]` | "Train Not Found!" shown; returns to the dashboard without changes |
| 14 | Change admin PIN (`C`) | Choose `C`, enter and confirm a new PIN | "Password Updated" if PINs match, "Pins Don't Match" otherwise |
| 15 | Cancel mid-edit (`D`) | Start entering any numeric field, press `D` | Field is cleared and entry restarts, or the current sub-menu is exited, without saving |
| 16 | Backspace (`C` during numeric entry) | While typing a numeric field, press a few digits then `C` | Last digit is removed from the LCD echo and from the value being built |

### Validation Procedure
- Cross-check the Clock view's time/date against an external reference (phone/PC clock) after any RTC edit.
- Physically verify LED color/state transitions match the configured train delay/arrival values.
- Confirm the buzzer is audible and pulses only while a train is in the "Arrived" state.
- Repeat the Admin Menu tests for a few different trains to confirm `trainDB.c` indexing and lookup by train number both work correctly.

---

## 🩹 Troubleshooting Guidelines

| Symptom | Likely Root Cause | Resolution |
|---|---|---|
| LCD shows no text / blank backlight | Incorrect wiring on RS/RW/EN or data bus pins; contrast (VEE) not set | Verify LCD pin connections against the [Pin Mapping](#-pin-mapping) table; adjust the LCD contrast potentiometer |
| LCD shows garbled/random characters | `InitLCD()` interrupted, or a command written before init completes | Ensure `InitLCD()` runs to completion in `application.c` before any other code writes to the LCD; check power supply stability |
| Clock drifts significantly over time | Incorrect `PREINT_VALUE`/`PREFRAC_VALUE` prescaler for your crystal frequency | Recalculate the RTC prescaler constants in `all_peripheral_defines.h` against your board's actual `F_OSC` |
| Keypad presses not detected / wrong key registered | Loose row/column wiring, or row/column pins swapped | Re-check wiring against `all_peripheral_defines.h` (`ROW0–ROW3` = P1.16–P1.19, `COL0–COL3` = P1.20–P1.23) |
| Admin Menu does not open when switch is pressed | External interrupt not configured/enabled, or switch wired to wrong pin | Confirm the switch is on P0.3 (EINT1) per `all_peripheral_defines.h`; verify `Init_EINT1()` is called in `main()` |
| Admin Menu opens repeatedly / switch feels "stuck" | Switch contact bounce not debounced in hardware or software | Add a debounce capacitor on the switch line, or add a software debounce delay in `eint1_sw.c` |
| Forgot the admin PIN | PIN was changed via option `C` and not recorded | If reachable, re-flash the firmware to restore the default `save_pass = 1234` in `application.c` |
| Yellow/Red LED stuck on with no train nearby | A train record has a stale `delayMinutes`, or the RTC date/time is incorrect | Use Admin Menu (`B → 2`) to reset the delay to 0, or correct the RTC via `A` |
| Buzzer stays on continuously | `Buzzer_Ctrl()` logic misbehaving, or a hardware short on P1.26 | Check the `hasArrival` buzzer pulse block in `application.c`; verify buzzer driver transistor/wiring |
| Train name doesn't scroll / scrolls garbage | `trainName`/`destination` not null-terminated, or entry exceeds the declared buffer size | Ensure all names in `trainDB.c` are valid, null-terminated C strings within `trainName[25]` / `destination[20]` |
| Board doesn't respond to Flash Magic / won't program | Board not in ISP mode, wrong COM port/baud selected, or DB9/UART wiring issue | Re-check ISP jumper/boot pins, confirm COM port in Device Manager, and reseat the DB9 cable |
| Numeric field won't accept input / edit doesn't "stick" | `D` was pressed (clears/cancels) instead of `*`/`#` (confirms) | Confirm the correct terminator key is used — `*`/`#` confirm and advance, `C` backspaces, `D` clears/cancels |

---

## 🌍 Real-Time Use Cases

1. **Small & Mid-Sized Railway Stations** — Replace static, manually-updated boards with a low-cost automated display that reflects delays instantly once staff update the delay value.
2. **Metro / Local Transit Platforms** — Adapt the same architecture for metro or bus-rapid-transit platforms where a small number of routes need live status display.
3. **Educational & Institutional Announcement Boards** — Repurpose the same clock + scrolling-message + status-LED framework for college bell schedules, bus arrival boards, or event countdowns.
4. **Industrial Shift/Process Timers** — The RTC + threshold + LED-alert pattern generalizes to any process that needs a "time until next event" and an escalating visual/audio alert (e.g., maintenance reminders, shift-change boards).
5. **Remote / Offline Deployments** — Because the system needs no network connectivity to function, it's suitable for stations or facilities with unreliable internet, where a staff member updates status locally via the keypad.
6. **Embedded Systems Teaching Aid** — Demonstrates, in one compact project, on-chip RTC interfacing, interrupt-driven input handling, keypad scanning, LCD driving, and simple real-time scheduling logic — useful as a reference implementation for coursework or interview discussion.

---

## 📸 Application Screenshots / Results

> Photos/screen captures of the running system demonstrating actual execution and validating results.

The following composite image consolidates all eight operational states of the system, from real-time clock display to admin menu navigation and train edit workflows.

<img width="1742" height="608" alt="resultofproject" src="https://github.com/user-attachments/assets/7a59e3d8-6a42-4e10-991c-ad45ecff952b" />



### 🗺️ Visual Validation Reference

| Location in Composite Image | Description of System State |
|---|---|
| **Top-Left** | LCD showing live time, day, and date during the Clock/Date view |
| **Top-Right** | Window 1 — LCD cycling through all trains with Green LED active |
| **Middle-Left** | Window 2 — LCD locked on the next upcoming train, Yellow LED active |
| **Bottom-Left** | Window 3 — LCD showing the active/arrived train, Yellow LED, buzzer active |
| **Bottom-Right** | LCD showing a delayed train's updated arrival/departure and Red LED active |
| **Upper-Middle-Right** | Admin PIN entry screen after pressing the edit switch |
| **Lower-Middle-Right** | Admin Dashboard (`A:RTC B:TrainDB C:Pass D:Exit`) |
| **All Edit-State Screens** | Sequence of LCD prompts while editing a train's platform/delay/timing |

*(Note: All results are validated within the single frame `images/results/system_overview_all.png`.)*

---

## ✅ Project Submission Checklist

As per the assigned project workflow, before submitting the GitHub repository link on the student portal, confirm:

- [ ] Project implemented and tested on real hardware within the given time frame.
- [ ] Repository includes all source files (`.c` / `.h`) and the Keil project (`.uvproj` / `.sct`).
- [ ] Repository includes system architecture details (block diagram + flow chart, as above).
- [ ] Repository includes hardware images/demo video.
- [ ] README file (this file) is complete and accurate, covering overview, features, structure, testing, troubleshooting, diagrams, use cases, and results.
- [ ] GitHub repository link submitted through the student login portal.
- [ ] Ready for hardware verification, GitHub review, and explanation/viva.
- [ ] Any modification requests from the review process are addressed and resubmitted.
- [ ] Final evaluation completed within the assigned project duration.

---

## 👤 Author

- **Student Name:** _[MD SHARIQUE]_
- **Roll No / ID:** _[V25HE10M13]_
- **Guide/Mentor:** _[Mr.Chandramouli Sir]_

---

## 📄 License

This project is submitted as part of an academic embedded systems course assignment.
