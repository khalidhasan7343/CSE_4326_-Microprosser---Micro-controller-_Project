# MediSmart embedded implementation evidence

Scope: all authored files in `medismart/medismartVscode`, the generated SDK configuration, and the two supplied wiring PNGs were reviewed. The PlatformIO `include`, `lib`, and `test` directories contain explanatory template READMEs, not application headers, libraries, or tests. This is a static source review, not an on-device experiment. Firmware and diagram facts below should be distinguished from measured performance and proposed improvements.

## Build and platform

- `platformio.ini:11–17`: environment `esp32dev`, platform `espressif32`, board `esp32dev`, framework `espidf`, serial monitor 115200 baud, DIO flash, and upload flash size 4 MB. Platform/framework versions are not pinned in this file.
- Root `CMakeLists.txt`: minimum CMake 3.16, ESP-IDF project include, project name `medismart`.
- `src/CMakeLists.txt`: recursively registers source files from `src` as an IDF component.
- Hidden `.vscode/extensions.json` recommends the PlatformIO IDE. Generated `.vscode/launch.json` and `c_cpp_properties.json` contain absolute paths from a different workstation; regenerate them through PlatformIO for the current checkout rather than treating them as portable setup instructions. `.gitignore` excludes `.pio` and these generated IDE artifacts.
- `sdkconfig.esp32dev:3,420,422`: generated configuration labels itself ESP-IDF 6.1.0 and targets ESP32; this labels the saved configuration, not a independently reproduced build.
- Saved SDK options: 160 MHz CPU (`:1665`), 100 Hz FreeRTOS tick (`:2246`), dual-core enabled (`:2244`), main task stack 3584 (`:1707`), main task affinity CPU0 (`:1709`), single application partition (`:713,725`).
- Configuration inconsistency: `platformio.ini` requests 4 MB upload flash, while saved `sdkconfig.esp32dev:676,690` selects 2 MB. Report this as a configuration discrepancy to reconcile before reproducing a build.
- `sdkconfig.defaults:1–2` and `sdkconfig.esp32dev:923–924` enable insecure TLS and skip server certificate verification.

## Hardware and pin map

| Subsystem | Device identified by project evidence | ESP32 signal | Source |
| --- | --- | --- | --- |
| Optical pulse/SpO2 | MAX30102 | GPIO22 SCL, GPIO21 SDA | `src/main.c:31–35`; sensor wiring PNG |
| ECG | AD8232 module identified in wiring PNG | GPIO34 analog OUT = ADC1 channel6; GPIO25 LO+, GPIO26 LO− | `src/main.c:36–39,299–304`; sensor wiring PNG |
| Saline mass | HX711 ADC and load cell | GPIO19 DT, GPIO23 SCK | `src/main.c:27–28`; sensor wiring PNG |
| Oxygen valve | MG996R servo | GPIO16, LEDC channel0 | `src/main.c:23,130`; actuator wiring PNG |
| Saline valve | Code labels MG996R; diagram labels SG90 | GPIO5, LEDC channel1 | `src/main.c:24,131`; actuator wiring PNG |
| Dose syringe | MG996R servo | GPIO18, LEDC channel2 | `src/main.c:25,132`; actuator wiring PNG |
| Alarm | Active buzzer identified in actuator wiring PNG | GPIO4 output, common GND | `src/main.c:26,135`; actuator wiring PNG |

Sensor wiring shows a shared 3.3 V supply/GND for MAX30102, AD8232, and HX711. Actuator wiring shows an external 5 V rail labelled “Battery / L298N”, with servo grounds and ESP32 ground connected. The code contains no L298N motor-control signals or software. The diagram is evidence of the intended supply arrangement, not a verified electrical schematic or proof of current capacity. The saline servo model remains contradictory and should be explicitly noted rather than silently chosen.

## Initialization and acquisition

- `init_hardware()` (`src/main.c:126–147`) sets the three LEDC channels to one shared 50 Hz, 13-bit, low-speed timer. It initializes HX711 SCK low and GPIO directions, then initializes I2C0 master at 100 kHz with GPIO internal pull-ups.
- Servo command formula (`:149–153`): pulse width in microseconds = `500 + ((2500 − 500) × angle / 180)` using integer arithmetic; duty = pulse width × 8192 / 20000. At 0° the duty is approximately 204; at 180° approximately 1024. Code commands a 0–180° span; no actual travel or mechanical limit is measured.
- `app_main()` (`:287–307`) initializes hardware, Wi-Fi, and network time, configures MAX30102 registers, configures ECG ADC and leads-off inputs, then starts a separate Firebase task.
- MAX30102 address is `0x57`. Raw configuration writes (`:294–296`) are reset `0x09←0x40`, 100 ms delay, FIFO config `0x08←0x4F`, SpO2 mode `0x09←0x03`, SpO2 config `0x0A←0x27`, and LED amplitudes `0x0C←0x24`, `0x0D←0x24`. These are code facts; any decoded electrical/sampling specifications should be supported by the manufacturer's datasheet.
- `max30102_read()`/`write()` (`:155–177`) use I2C commands with 50 ms timeout but discard returned error codes.
- Every main-loop iteration reads one six-byte optical FIFO record from register `0x07` (`:332–335`), splitting it into 18-bit RED and IR values. It does not check FIFO pointers, availability, overflow, sensor identity, or signal validity.
- ECG ADC is configured on ADC1 channel6 with default bit width and 12 dB attenuation (`:299–303`). If either leads-off input is high, the transmitted value is forced to zero; otherwise one raw ADC value is read (`:326–330`). No ADC-to-millivolt conversion, calibration API call, ECG analysis, or waveform buffer is implemented. “ECG” therefore means a raw ADC sample, not a diagnostic ECG result.
- `read_scale()` (`:184–197`) waits at most about 10000 microseconds for DT low; timeout returns zero. It clocks 24 bits using one-microsecond half-cycle delays and emits a 25th clock pulse, toggling the sign bit with XOR `0x800000` before return. The exact gain selected by 25 pulses should be attributed to the HX711 datasheet if stated.

## Saline estimate

For nonzero scale return values, firmware computes (`src/main.c:315–324`):

`x = max(0, (raw_weight − 8388608) / 1000)`

`current_ml = x` when the previous estimate is zero, otherwise `current_ml = 0.85 × current_ml + 0.15 × x`.

The firmware labels this value mL. There is no measured calibration coefficient, empty-container tare, solution density, load-cell capacity/model, calibration procedure, or calibration dataset in the reviewed embedded materials. Thus this is a code-level proxy for remaining volume with an assumed conversion coefficient, not established volumetric accuracy. Timeout retains the prior value without a validity flag.

## Pulse and oxygen algorithms

Parameters (`src/main.c:49–53`): oxygen trigger 85.0%, optical finger threshold IR=20000, accepted BPM strictly greater than 50 and strictly less than 130, eight-entry BPM buffer.

When IR is below threshold (`:337–340`), BPM and SpO2 are reset to zero, DC values and last beat time are reset, and the BPM buffer is cleared. The AC extrema, previous AC sample, and `beat_armed` are not reset in this branch.

For finger-present samples (`:342–385`):

1. DC estimates: `DC_IR = 0.95 × DC_IR + 0.05 × IR`; similarly for RED. AC is raw value minus DC.
2. Track AC minima/maxima. Dynamic detection threshold is `0.5 × max_AC_IR`.
3. Arm a beat on upward threshold crossing, then detect its peak when IR AC begins decreasing. Require more than 400 ms since last accepted peak time.
4. Beat estimate is `BPM = 60000 / Δt_ms`. Buffer only estimates in the strict interval `(50,130)`; peak timestamp is advanced even for rejected BPM.
5. When at least five buffer entries are positive, sort them, remove the largest and smallest, and average the rest. Initialize display BPM to this trimmed mean, then use `display_BPM = 0.97 × previous + 0.03 × trimmed_mean`.
6. Let `A_IR = max_AC_IR − min_AC_IR` and similarly for RED. For positive amplitudes, ratio `R = (A_RED/DC_RED)/(A_IR/DC_IR)`, approximate SpO2 `110 − 25R`, clamp to `[80,100]`, then initialize or smooth with `SpO2 = 0.9 × previous + 0.1 × estimate`.

The SpO2 relation is an empirical formula in code; there is no reference-oximeter comparison or subject study here. The 80% lower clamp prevents display of estimates below 80%. Extrema are cleared at beat detection before the later same-iteration SpO2 computation, so beat-window bookkeeping may distort estimates. No explicit DC_RED positivity check, motion-artifact rejection, confidence metric, or validated calibration curve is implemented.

## Control states and nominal timing

The main loop ends with `vTaskDelay(20 ms)` (`src/main.c:457`), so the delay alone suggests at most about 50 iterations/s; actual loop periods include sensor I/O and processing. Treat all derived times below as nominal lower bounds, not measured latency.

Saline (`:392,397–405`): enable threshold only if `saline_stop_ml > 0`; closure condition is strictly `current_ml < saline_stop_ml`. Increase angle by 2°/iteration to 180°, status `Closing` then `Stopped`. Otherwise decrease by 2° to 0°, status `Opening` then `Open`. Nominal full travel is 90 iterations or approximately 1.8 s plus loop overhead. Equality does not close it. There is no hysteresis or permanent stop latch; a rising measured volume reopens the valve.

Dose (`:393–394,408–433`): weight trigger requires positive `dose_trigger_ml` and strictly lower current volume. Time trigger requires configured hour equal local hour and configured minute equal local minute. The triggers are ORed. Two booleans implement standby, forward stroke, and return. On trigger while inactive, increase angle 1°/iteration from 0° to 180°, status `Pushing`. On the next iteration after reaching 180°, set `dose_returning`, status `Pushed`, local weight trigger to zero, and a flag requesting cloud reset. Return by 2°/iteration; status remains `Pushed` during return, then becomes `Standby`. Nominal forward movement is 180 iterations (~3.6 s), return 90 iterations (~1.8 s), with extra transition iterations and sensor overhead. This executes a fixed full servo stroke; no user-defined drug volume, plunger travel calibration, pressure limit, limit switch, or dose confirmation is implemented.

A significant scheduling defect follows directly from these conditions: the time trigger stays true for every iteration during the matched minute, and neither a once-per-schedule execution latch nor schedule reset is implemented. After one cycle completes, another can start during the same minute. Time is not validated as synchronized before use. Schedule parsing accepts integer hour/minute without range validation; clearing or removing `dose_time` does not clear an already remembered valid schedule unless new valid parsing updates it.

Oxygen/alarm (`:395,435–445`): trigger only when estimated SpO2 is positive and strictly below 85%. Drive buzzer high and raise oxygen servo angle by 2°/iteration toward 180°. Otherwise buzzer low and return by 2° toward 0°. Nominal full travel is ~1.8 s plus loop overhead. Finger loss yields SpO2 zero and disables this alarm condition. No measured oxygen flow, proportional oxygen control, hysteresis, latching alarm, oxygen valve position confirmation, or patient-specific threshold is provided.

## Wi-Fi, time, and Firebase protocol

- `init_wifi()` and event handler (`src/main.c:84–115`) create an ESP32 station, connect on start, attempt reconnection up to 100 times after disconnection, and reset retry count after IP acquisition. No explicit application-level connection wait or offline/stale-data status is included.
- `init_time()` (`:117–123`) starts poll-mode SNTP with `pool.ntp.org` and sets POSIX timezone string `UTC-6`, corresponding to UTC+06:00. There is no wait for valid synchronization, last-dose date, or battery-backed RTC shown.
- `firebase_sync_task()` (`:214–284`) uses 512-byte RX/TX buffers, receives HTTP chunks into a global RX buffer, GETs global `/settings.json`, optionally PATCHes its dose reset, then PATCHes global `/vitals.json`, then delays 2000 ms. Synchronization is approximately one cycle per 2 s **plus HTTP request durations**, not a guaranteed exact 0.5 Hz.
- Task creation: 4096 stack size, priority5 (`:306`). Main acquisition/control continues in the main task while networking blocks in the separate task.

Settings read (`:233–249`):

| JSON key | Parsed meaning |
| --- | --- |
| `saline_stop_ml` | Positive cutoff for estimated remaining saline |
| `dose_trigger_ml` | Positive remaining-volume threshold that starts fixed syringe stroke |
| `dose_time` | Parsed `HH:MM` daily local-time match |

Latest vitals PATCH payload (`:270–272`):

```json
{
  "bpm": "numeric with one decimal place",
  "spo2": "numeric with one decimal place",
  "ecg": "integer raw ADC sample",
  "current_ml": "numeric with one decimal place",
  "saline_status": "Open / Opening / Closing / Stopped",
  "dose_status": "Standby / Pushing / Pushed"
}
```

The above uses descriptive strings for numeric types; actual firmware transmits JSON numbers. At the forward-stroke end, firmware PATCHes `{"dose_trigger_ml":0}` into settings (`:255–266`). It does not reset `dose_time`. No oxygen status, servo angle, timestamp, sensor validity, ECG sample series, device ID, patient ID, or acknowledgement is transmitted. Firmware reads/writes one shared global dataset; per-patient routing is absent.

The client uses `strstr`, `strchr`, `atof`, and `sscanf` instead of a JSON parser. It does not verify HTTP status codes, validate threshold bounds, or check write success before clearing the reset flag. A missing/malformed settings key retains its previous value. Error handling and settings validity are limited. There is no REST auth token or Firebase Authentication flow in the device code.

## Evidence-based limitations and future-work targets

1. No authored embedded tests or experimental datasets, physical calibration records, logged actuator trials, timing measurements, ECG accuracy checks, or SpO2/BPM reference comparisons appear in the reviewed embedded folder. Do not claim clinical accuracy, measured improvement, successful trials, or deterministic response from source review alone.
2. Sensor/I2C/ADC/LEDC initialization errors are largely ignored. Failed optical reads can leave a local six-byte buffer without reliable values. Scale timeout silently preserves stale volume. Raw ECG zero merges leads-off with a possible actual zero ADC value.
3. Shared telemetry values, thresholds, status strings, and dose reset flag cross task boundaries without a mutex or snapshot. A telemetry packet can mix values from different main-loop moments, and dose/settings updates can race.
4. Cloud settings are accepted without authorization context, range limits, versioning, acknowledgements, or stored safe defaults. This is particularly relevant because actuator decisions use remote values directly.
5. TLS server verification is explicitly disabled. Wi-Fi credentials are hard-coded in `src/main.c:42–43`; their values must not be printed in the report. Connection endpoints are embedded too. Do not reproduce credential source lines or unredacted source listings.
6. There is no local persistent history or reconnect upload queue. The firmware PATCH updates latest shared values; waveform history and multi-patient operation are not device-side features.
7. Thresholds lack hysteresis/debounce, and sensor loss is not a distinct alarm condition. Actuator positions are assumed from software angles; no flow, pressure, encoder, or limit-switch feedback is implemented.
8. Time-trigger dose repetition, retained schedules after invalid/cleared input, and lack of synchronized-time validation should be reported as present limitations rather than treated as completed scheduling safeguards.
9. Mechanical dose-volume and oxygen-flow claims require additional instrumentation and calibration; the implementation establishes actuator commands only.
10. The saved flash-size mismatch and saline servo model discrepancy should be resolved before a reproducibility or bill-of-materials claim is finalized.

## Suggested report framing

Describe the supplied design as an educational integrated monitoring-and-actuation prototype with ESP32 sensing, local threshold/state-machine control, REST cloud synchronization, and web-role interfaces documented separately. State explicitly which conclusions come from implementation inspection, which remain proposed test plans, and which require additional measurements. Explain the calibration assumptions, nominal rather than measured timings, and clinical-validation boundary without inventing results.
