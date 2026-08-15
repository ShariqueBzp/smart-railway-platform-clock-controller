# LPC2148 Smart Railway Platform

An advanced embedded systems project developed for the **ARM7 LPC2148 microcontroller**. This system provides real-time train schedule monitoring, dual-mode LCD view switching, visual/audio status notifications, and a secure password-protected administrator dashboard for on-the-fly railway schedule adjustments.

---

## **Key Features**

* **Real-Time Clock (RTC) Integration**: Maintains precise time and date in strict 24-hour railway format.
* **Dual-Mode Display Scheduling**:
* *Window 1*: Cycles through all active train schedules.
* *Window 2*: Locks onto upcoming trains based on real-time comparison.
* *Window 3*: Displays live arrival and departure countdown statuses.


* **Secure Admin Dashboard**: Accessed via an external hardware interrupt (`EINT1` on `P0.3`) and secured by a PIN-based authentication layer.
* **Dynamic Schedule Management**: Allows authorized personnel to update platforms, apply delay offsets, or directly modify arrival and departure times.
* **Interactive Peripherals**: Features an 8-bit mode 16x2 LCD display and a 4x4 matrix keypad for user navigation and data entry.
* **Status Indication**: Integrated LED and buzzer actuation for instant alerts (On-Time, Approaching, Delayed).

---

## **Project File Structure**

```text
├── application.c         # Main application loop, state machine, and mode switching
├── scheduler.c / .h      # Multi-window display logic and schedule time comparisons
├── menu.c / .h           # Admin dashboard, password auth, and record editor
├── rtc.c / .h            # Real-Time Clock configuration and register management
├── lcd.c / .h            # 16x2 LCD 8-bit mode driver routines
├── kpm.c / .h            # 4x4 matrix keypad scanning and parsing routines
├── led_buzz.c / .h       # GPIO atomic drivers for LEDs and buzzer indicators
├── eint1_sw.c / .h       # External interrupt handler for admin mode activation
├── trainDB.c / .h        # Structured database for train schedules and delay calculations
└── delay.c / .h          # Precise microsecond, millisecond, and second timing functions

```

---

## **System Architecture & Workflow**

1. **Initialization**: Configures internal system clocks, GPIO pins, the LCD interface, the matrix keypad, and the onboard RTC.
2. **Main Operation Loop**: Continuously compares current RTC time with the `trainDB` entries. The LCD dynamically updates schedule statuses across the configured windows.
3. **Interrupt Handling**: Pressing the external interrupt button (`EINT1`) triggers an interrupt service routine that safely halts regular scheduling display views and prompts for the admin PIN.
4. **Admin Control**: Once authenticated via the 4x4 keypad, administrators can update:
* RTC time (Hours, Minutes) and date components.
* Train platform numbers.
* Train delay intervals (with automatic arrival/departure recalculation).
* Direct arrival and departure times.
* The administrator security PIN.
