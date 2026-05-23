# Boot Test

**Firmware Test:** `firmware/tests/boot_test/`  
**Status:** Complete  
**Toolchain:** Arduino IDE  
**Target Hardware:** ESP32-WROOM-32 (ESP32-D0WD-V3 rev 3.1)

---

## Purpose

Validates that the ESP32 powers on correctly, the Arduino IDE toolchain can successfully compile and flash firmware, and the onboard LED responds to GPIO control. This is the baseline hardware and toolchain confidence check before any sensor or connectivity code is introduced.

---

## What It Tests

| Check | Description |
|---|---|
| Serial output | UART initializes at 115200 baud and prints to Serial Monitor |
| CPU frequency | Reports clock speed via `getCpuFrequencyMhz()` |
| Flash size | Reports flash memory size via `ESP.getFlashChipSize()` |
| Free heap | Reports available RAM via `ESP.getFreeHeap()` |
| Chip revision | Confirms chip revision via `ESP.getChipRevision()` |
| GPIO blink | Toggles GPIO 2 (onboard LED) HIGH/LOW on 500 ms interval |

---

## Expected Serial Output

```
=== ESP32 Boot Test ===
Boot successful!
CPU Freq: 240 MHz
Flash Size: 4 MB
Free Heap: 327668 bytes
Chip Revision: 3
=======================
LED ON
LED OFF
LED ON
LED OFF
...
```

> Free heap will vary slightly between runs. Chip Revision 3 corresponds to the ESP32-D0WD-V3 rev 3.1 used in this build.

---

## Hardware Required

- ESP32-WROOM-32 development board
- USB cable (data-capable)
- No external components required — uses onboard LED on GPIO 2

---

## How to Flash

1. Open `boot_test.ino` in Arduino IDE
2. Select **Tools → Board → ESP32 Dev Module**
3. Select **Tools → Port → COM4** (CP210x — confirm in Device Manager)
4. Close Serial Monitor if open (it will lock the COM port)
5. Click **Upload**

---

## How to Verify

1. After upload completes, open **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press the EN (reset) button on the ESP32 if output does not appear
4. Confirm the header block prints and LED blink messages repeat
5. Visually confirm the onboard LED blinks at ~1 Hz

---

## Pass Criteria

- [ ] Serial output prints without garbage characters
- [ ] CPU Freq reports `240 MHz`
- [ ] Flash Size reports `4 MB`
- [ ] Free Heap is a non-zero value (typically 300 000 – 330 000 bytes)
- [ ] Chip Revision reports `3`
- [ ] Onboard LED blinks visibly at 500 ms on / 500 ms off

---

## Known Notes

- **COM port:** COM4 is the CP210x (ESP32). COM3 is Intel AMT — do not use.
- **PowerShell:** If running `esptool.exe` manually, use `cmd` rather than PowerShell; PowerShell mishandles `--` flags.
- **Serial Monitor lock:** The COM port cannot be used for flashing while Serial Monitor is open. Always close it before uploading.
- **PlatformIO:** Not used for this project. The espressif32 framework (tested at v7.0.1 and v6.4.0) produces binaries incompatible with the ESP32-D0WD-V3 rev 3.1. Arduino IDE is the confirmed working toolchain.

---

## Related Documents

- `docs/decisions/` — Toolchain decision log (PlatformIO → Arduino IDE)
- `docs/bom/` — Bill of materials including ESP32 module
- `firmware/tests/` — Index of all firmware test sketches
