# Website evidence review

Reviewed: all seven `medismart/public/*.html` files, `medismart/firebase.json`, and all seven `Website_images/*.jpeg` screenshots. Application files were not modified.

## Implemented functions

- `register.html:28,52,68,71`: self-selected patient/family/doctor account; Firebase email/password registration; `/users/<uid>` record containing `email` and `role`.
- `login.html:43,57,60,64,71`: email/password sign-in; reads the role record; redirects to the appropriate HTML page. Missing role signs the user out.
- `patient.html:30,56,60,64`: BPM and SpO2 display, signed-in check and logout.
- `family.html:30,60,64,68,78`: BPM and SpO2 display, signed-in check and logout, flashing browser warning.
- `doctor.html:24,61,73,85,144,168,177`: BPM, SpO2, saline estimate, valve/syringe statuses, ECG chart, threshold edits and manual controls.
- `firebase.json:2`: static Firebase Hosting from `public`. The root `index.html` is still a generated Firebase welcome page; `404.html` is a generated missing-page response.

All dashboards read the same `/vitals` path, and doctor controls write the same `/settings` path. No patient/device partition, doctor-patient assignment, family-patient link, medication schedule, historical database log, email, SMS or push delivery is implemented.

## Data schema

```text
/users/<uid>: email, role
/vitals: bpm, spo2, ecg, current_ml, saline_status, dose_status
/settings: saline_stop_ml, dose_trigger_ml
```

The database screenshot also contains `saline_ml` and `saline_threshold` under `/vitals`; the current website does not read these fields.

## Programmed display thresholds and controls

Patient BPM: zero shows sensor-ready/place-finger; 60-100 inclusive shows normal; all other nonzero values show elevated, including low readings. Patient SpO2: zero shows sensor-ready/place-finger; values at least 95 show optimal; others show low. These are prototype UI criteria, not validated diagnostic rules (`patient.html:74,86`).

Family warning condition: BPM above 120, positive BPM below 50, or positive SpO2 below 92. Zeros are excluded. The warning is an on-page animated banner (`family.html:15,78`).

Doctor ECG warning: `ecg === 0`. Doctor low-oxygen warning: positive SpO2 below 85. Its message mentions emergency oxygen actuation, but this page does not issue an oxygen command (`doctor.html:154`).

Doctor settings actions (`doctor.html:177,183,187,191`): Sync writes both parsed thresholds, blank/invalid values become zero; Open Valve writes `saline_stop_ml=0`; Close Valve writes `saline_stop_ml=9999`; Reset Dose writes `dose_trigger_ml=0`. Writes have no success/failure UI. Status labels reflect subsequent device telemetry, not a command acknowledgement.

## ECG plot

The Chart.js plot holds 50 values, starts with zeros, and appends one ECG value for each `/vitals` value event. The axis range is 0-4095; the horizontal axis and labels are hidden. No timestamps, voltage calibration or fixed sampling interval are represented (`doctor.html:111,135,158`). Describe as a rolling received-value visualization.

## Screenshot observations

| Image | Observation |
|---|---|
| `web_1.jpeg`, `web_6.jpeg` | Doctor: BPM 0, SpO2 0, saline 324.1 mL, stop setting 320 mL, Valve Open, Syringe Standby, ECG-disconnection banner, zero trace. Images visually repeat the same state. |
| `web_2.jpeg`, `web_7.jpeg` | Family: BPM 0, SpO2 0, no warning. Images visually repeat the same state. |
| `web_3.jpeg` | Database corroborates dose trigger 0, stop setting 320, current_ml 324.1, BPM/SpO2/ECG 0, valve Open and syringe Standby. |
| `web_4.jpeg` | Registration role dropdown. |
| `web_5.jpeg` | Patient zeros and sensor-ready/place-finger badges. |

These establish rendered screens and browser/database consistency in an idle/unavailable-sensor state. They do not establish physiological accuracy, successful dose delivery, valve stopping below a threshold, calibration error or response time. No random/synthetic value generator appears in the browser code; it renders database values, whose measurement provenance must be checked in the firmware.

## Limitations to report

1. Registration directly accepts the selected doctor role; no documented approval/verification.
2. Patient/family pages check sign-in, not saved role.
3. Doctor page has no Firebase Authentication import or auth/role check.
4. Doctor configuration retains placeholder identifiers (`doctor.html:98`); screenshots may reflect a separately configured copy.
5. Shared data paths represent one common prototype stream.
6. Threshold inputs have no bounds checks; invalid/blank inputs become zero.
7. No telemetry timestamps, connection status, stale-data warning or listener error handling.
8. No command IDs, acknowledgements, audit trail or logged history.
9. No database rules are supplied; server-side access protection is unknown. Do not assert the live database is public or secured.
10. Root URL shows the Firebase template, so the operational entry is `login.html`.

No author names, GitHub repository link, YouTube demonstration link or experiment statistics occur in the website sources. Configuration values have been omitted from these notes.
