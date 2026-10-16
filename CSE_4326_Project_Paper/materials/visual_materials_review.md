# Slides and visual evidence review

This review records the supplied slide deck, every project photograph, every web screenshot, and the two wiring diagrams. It distinguishes observed artifacts from presentation claims. Coordinates below use the upper-left image corner as the origin; percentages are approximate positions for later figure annotations.

## Presentation metadata

The seven-slide presentation is titled **Autonomous Smart Dual Infusion & Patient Monitoring System Using ESP32**. Slide 1 names the following team members:

| Member | Student ID |
|---|---|
| Md. Turjo Hasnat | 0112410067 |
| Md. Khalid Hasan | 0112410009 |
| Moieb Al Jawad | 0112410289 |
| Swarup Deb Nath | 0112230150 |
| Md Shakib | 0112410546 |

The presentation does not provide a university, department, instructor, supervisor, email address, course code, GitHub link, or YouTube link. The only external relationship in the PPTX is a Gamma branding URL. Notes contain slide numbers 1–7 and no substantive additional information. Full extracted text is in `slides_text.txt`.

All raster illustrations on the title, problem, hardware-list, architecture, working-principle, and conclusion slides were also inspected. They are stylized decorative drawings/icons, not photographs, datasheets, readable hardware model labels, experimental plots, or independent evidence. Extracted copies are retained in `presentation_assets/`, with architecture/working-principle backgrounds beside this review. An apparent `E5982` string on the stylized controller icon is decorative text and must not be treated as the ESP32 model number.

## All seven slides

1. **Title and team.** Describes an ESP32-based IV infusion, syringe pump, and patient monitoring solution, aiming at reliable and affordable bedside automation.
2. **Problem and objectives.** Manual IV and syringe control can vary and cause delays; continuous monitoring is limited; repetitive tasks raise nurse workload; integrated low-cost solutions are needed in resource-constrained hospitals. Objectives are automatic IV and syringe delivery, MAX30102 heart-rate monitoring, ESP-NOW/Wi-Fi dashboard telemetry, alerts, LCD status, and reduced workload.
3. **System architecture.** ESP32 is the central controller; patient signals feed monitoring; MAX30102 provides HR and SpO2; an IR sensor and servo serve the IV module; a stepper and lead screw serve the syringe module; LCD and buzzer provide local status. Bidirectional remote controls and telemetry are asserted. The extracted raster `slide3_image4.png` is merely a decorative six-node background: text labels live in slide XML and are absent from the raster.
4. **Hardware list.** ESP32 DevKit, MAX30102, IR drop sensor, IV servo, stepper and lead screw, LCD and buzzer, motor driver and power supply, rechargeable battery and wheels, following sensor/camera. Quantities, costs, exact motor models, and camera models are absent.
5. **Working principle.** Describes sensor feedback closing a loop through ESP32, with drop detection, heart-rate monitoring, servo adjustment, and a stepper syringe. Mentions safety checks, alarms, and streaming. `slide5_image14.png` is a decorative five-circle graphic with separate labels in XML; it is unsuitable as a standalone architecture diagram.
6. **Features and advantages.** Asserts simultaneous control of two IV lines, automatic IV/syringe actuation, closed-loop feedback, trend alerts, ESP-NOW/Wi-Fi, precision and consistent dosing, lower error/workload, portability, and Bangladesh-hospital suitability. It mentions LCD and web dashboard. No experimental data or measured comparisons accompany these claims.
7. **Conclusion/future scope.** Claims integration and benefits for safety/workload. Future proposals: AI prediction/anomalies, BP/SpO2/ECG sensors, app, and cloud records. This is partly stale relative to the supplied implementation: SpO2, AD8232 ECG, and Firebase cloud functions already appear in wiring/code/UI artifacts.

## Project photographs

All photos except 6 are 960 x 1280; photo 6 is 773 x 1280. They document a handmade pole-mounted prototype with exposed electronics; they do not document testing on a patient.

| File | Observed content and figure value | Suggested annotations |
|---|---|---|
| `Project_images/1.jpeg` | Close view of a breadboard enclosure on the wheeled base. Useful electronics assembly figure, though many wires obscure board identifiers. Visible ECG electrode pads hang from the enclosure. | Main ESP32-like controller: x=49–64%, y=61–69%. Breadboard enclosure: x=18–75%, y=52–81%. Electrode pad/cable: x=12–26%, y=62–75%; another pad x=22–34%, y=79–88%. Metallic cylindrical component at lower right has no readable model; do not label its exact role. Pole/wiring: x=43–55%, y=0–57%. |
| `Project_images/2.jpeg` | Close view of clear face mask and tubing on the left, dark cylinder/body and servo/regulator assembly on the right. A two-transducer ultrasonic-style sensor is mounted above. Useful oxygen module photo; neither calibrated oxygen delivery nor cylinder safety is demonstrated. | Ultrasonic-style module: x=53–64%, y=12–20%. Clear tubing chamber: x=19–36%, y=15–39%. Face mask: x=16–43%, y=39–65%. Servo/regulator assembly: x=52–67%, y=40–58%. Dark cylinder-like body: x=52–70%, y=28–96%. Exact cylinder substance, pressure, and servo model cannot be verified from the photograph. |
| `Project_images/3.jpeg` | Best close-up of syringe mechanics. A vertical syringe sits in a handmade guide frame, with a visible rack-and-pinion gear at the upper end. A servo is attached at lower right and an ultrasonic-style module is at lower left. The photo does not visibly support a stepper-and-lead-screw mechanism. | Rack and gear: x=47–64%, y=21–38%. Syringe/guide: x=44–63%, y=35–84%. Servo: x=60–72%, y=72–86%. Ultrasonic-style module: x=26–33%, y=86–96%. Fluid tube: x=36–59%, y=42–93%. |
| `Project_images/4.jpeg` | Best load-cell close-up. A metal load cell clearly bears a 5 kg label and arrow. A white hook hangs beneath it, with wires and a small PCB behind the upper pole. Suitable for explaining bottle-weight measurement, rather than optical drop counting. | 5 kg load cell: x=39–55%, y=35–43%. Hook: x=42–55%, y=45–60%. Upper PCB/wiring: x=52–65%, y=30–42%. Small hanging PCB at left: x=14–31%, y=58–65%; exact identity unreadable. |
| `Project_images/5.jpeg` | Base-mounted red motor-driver board with blue screw terminals and an Arduino-like blue secondary controller board below, plus wiring. Driver resembles an L298N module, but exact identification should be described as visual inference unless supported by wiring labels. Useful photo for mobility hardware, which has no supplied firmware. | Driver board: x=33–54%, y=26–40%. Secondary controller: x=32–56%, y=61–81%. Small blue lower PCB: x=22–36%, y=86–95%. Enclosure edge: x=2–81%, y=0–15%. Wheels/motors are mostly outside the frame. |
| `Project_images/6.jpeg` | Best full-system photograph, showing vertical stand, top hook/load-cell assembly, mask, mounted modules and enclosure on a caster base. Recommended primary prototype overview. | Top load-cell/hook: x=44–54%, y=14–21%. Mask/tubing: x=40–51%, y=29–43%. Lower mounted cylinder/module: x=48–60%, y=48–69%. Electronics enclosure: x=35–55%, y=78–91%. Wheeled base: x=28–76%, y=83–98%. |

The photographs show neither a clearly readable LCD nor a distinct verified IR drop detector or camera. Two ultrasonic-style modules are visible, supporting proximity/distance hardware rather than a confirmed camera. Do not claim successful autonomous following from these photos.

## Website screenshots

| File | Visible evidence | Interpretation |
|---|---|---|
| `Website_images/web_1.jpeg` | Doctor/clinical control interface: HR=0 BPM, SpO2=0%, saline=324.1 mL, stop threshold input=320, valve Open, syringe Standby; sync thresholds, manual valve buttons, reset dose; disconnected ECG banner and zero ECG trace. | Confirms interface layout and a cloud-fed state. Zero BPM/SpO2 and disconnected ECG do not establish valid physiological measurements. Current 324.1 mL > 320 mL is consistent with firmware's rule to close only when remaining-volume estimate is strictly below threshold. |
| `Website_images/web_2.jpeg` | Family/guardian view with HR=0 and blood oxygen=0. | Confirms read-only family interface appearance, not vital accuracy. |
| `Website_images/web_3.jpeg` | Firebase Realtime Database tree with settings and vitals. Publicly visible values include `dose_trigger_ml=0`, `saline_stop_ml=320`, `bpm=0`, `current_ml=324.1`, `dose_status=Standby`, `ecg=0`, `saline_status=Open`, `spo2=0`. Also contains user IDs and project endpoint. | Confirms documented schema/state only. Prefer doctor screenshot and schema diagram in report; avoid reproducing user IDs or unnecessary endpoint details. |
| `Website_images/web_4.jpeg` | Create Account dropdown offering Patient, Family / Guardian, and Medical Doctor, with password field and sign-up/sign-in controls. | Confirms self-selected roles in registration UI; does not demonstrate backend enforcement of authorization. |
| `Website_images/web_5.jpeg` | Patient dashboard with HR=0 BPM, oxygen=0%, and Sensor Ready / Place Finger labels. | Confirms no-finger display state and role-specific UI. |
| `Website_images/web_6.jpeg` | Byte-identical duplicate of `web_1.jpeg`. | Do not include twice. |
| `Website_images/web_7.jpeg` | Byte-identical duplicate of `web_2.jpeg`. | Do not include twice. |

Recommended UI figures: doctor `web_1`, patient `web_5`, guardian `web_2`, registration `web_4`. The console screenshot is optional and contains user identifiers; describe the schema textually instead.

## Wiring diagrams

`medismart_sensor_wiring.png` is a sensor wiring sketch, not a complete circuit schematic. All pictured sensors use a 3.3 V rail and common ground. It specifies MAX30102 SCL=GPIO22/SDA=GPIO21; AD8232 OUT=GPIO34, LO+=GPIO25, LO-=GPIO26; HX711 SCK=GPIO23, DT=GPIO19. It does not show the four actual load-cell bridge wires or module-level regulator/pull-up detail.

`medismart_power_actuators_wiring.png` shows a 5 V supply labelled Battery / L298N, common ground, saline valve SG90 at GPIO5, dose syringe MG996R at GPIO18, oxygen valve MG996R at GPIO16, active buzzer GPIO4/GND. It does not specify supply capacity, fuse, battery voltage, regulator topology, or protected motor-driver wiring. The supplied firmware comments describe all three servos as MG996R, so the saline SG90 label is inconsistent. Use a generic saline servo label unless the actual model is verified. These PNGs have transparent backgrounds and some dark lines/labels become hard to read against black; use a white page background when embedding.

`CSE_4326_Project_Paper/fig1.png` is a template example (black dot on white), not a project diagram. Exclude it from the report.

## Claims requiring qualification

- Slides describe IR drop counting, stepper plus lead screw, LCD, ESP-NOW, two IV lines, and following/camera. The supplied ESP32 source instead implements HX711-based weight estimate, three servo PWM outputs, MAX30102, AD8232, Wi-Fi/HTTPS/Firebase. Photos visibly show rack-and-pinion syringe mechanics. Treat slide-only features as concept/proposed scope unless other evidence establishes implementation.
- One saline path plus a separate syringe is evidenced; two simultaneous independently regulated IV paths are not established.
- The weight-to-mL conversion in source uses a fixed offset and divisor without provided calibration records; the 324.1 mL screenshot is a reported estimate, not an independently validated volume.
- The syringe code performs a servo angle cycle. A target mL threshold triggers a push; it is not a calibrated mL dosage command and does not prove delivered-volume accuracy.
- Measured dosing error, flow accuracy, latency, battery life, nurse time saved, physiological accuracy, cost, and successful bedside safety outcomes are absent from the supplied slides/images. No clinical evaluation should be inferred.
- SpO2/ECG/cloud are called future work in slides while existing in code/UI; use the code/UI to describe current implementation and reserve future work for validation and refinement.
