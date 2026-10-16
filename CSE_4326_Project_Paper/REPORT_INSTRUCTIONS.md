# Report instructions and submission checklist

This document preserves the requirements embedded in the original IEEE report template and explains how the six report chapters address them. The untouched source is retained as [materials/original_report_template.tex](materials/original_report_template.tex). The editable report entry point is [conference_101719.tex](conference_101719.tex). The template's six numbered sections are treated as chapters while retaining IEEE conference formatting.

## Front matter

- Use the project title and five team members established by the supplied slide deck. Replace example authors and example title text. The slide title is **Autonomous Smart Dual Infusion & Patient Monitoring System Using ESP32**; implementation details must follow the actual source and physical evidence even when the slide title suggests broader capabilities.
- The original template's author blocks request each member's name, department, university/organization, city/country, and email or ORCID. Student IDs are available in the slides. The user has now confirmed **BSCSE**, **United International University**, and supervisor **Arnab Bhattacharjee**, and explicitly requested removal of the city/country and email/ORCID lines. Those lines are therefore omitted under the current user override; they do not remain pending placeholders.
- The template warns that subtitles are not captured in Xplore. Avoid an unnecessary subtitle. Include a first-page funding footnote only if funding is confirmed; remove the example funding text otherwise.
- The abstract must contain **1–2 sentences on the problem, 2–3 sentences on the implemented solution, and 1–2 sentences on performance/accuracy/efficiency**. Performance sentences must state the actual available evidence and measurement limitations. No fabricated numerical accuracy or efficiency claims are permitted.
- Replace example keywords with project-specific keywords such as ESP32, patient monitoring, infusion prototype, load cell, pulse oximetry, ECG, and Firebase.

## Chapter 1 — Introduction

The template requires three topics:

1. **Problem statement and motivation:** explain the project idea, the practical problem, why it matters, and why the team chose it. Relate the monitoring/control prototype to bedside workflow without claiming demonstrated clinical benefit.
2. **Literature review:** find related published work, explain its implemented features, and cite it. The original template suggests Google, Google Scholar, or ResearchGate; the completed report should cite the original paper/publisher/source rather than a search result. Use authoritative datasheets and documentation for device-specific claims.
3. **Innovative features:** compare this implementation with prior work and summarize the project's features. Describe its integration choices precisely; avoid claiming that a feature has never existed unless evidence supports that claim.

Implementation scope must be reconstructed from firmware, website, photographs, and wiring. Slide-only claims such as a stepper/lead-screw drive, IR drop counting, LCD, ESP-NOW, or two independent IV channels require supporting evidence before being called implemented.

## Chapter 2 — Proposed Method

- Include a readable block diagram of the **whole system**, showing each sensor, controller, actuator, power connection, and remote software/service as applicable. PowerPoint is suggested by the template, but another tool is allowed.
- Label signal direction and connections in plain language so a nontechnical reader can follow operation. Show the MAX30102, AD8232, HX711/load cell, ESP32, servo actuators, buzzer, Wi-Fi/Firebase, and role-specific browser views when supported by source.
- Explain each block, what it measures or controls, how it connects to other blocks, and how the complete system operates. Distinguish power paths, sensor readings, commands, and telemetry.
- Document the control rules and processing: remaining-volume estimate, saline cutoff, syringe trigger/cycle, oxygen-related alarm rule, sensor handling, and cloud polling. A threshold that initiates a servo cycle is not a measured delivered-dose command.
- The slides' decorative raster backgrounds are not complete standalone block diagrams; their labels are separate slide objects. Use a complete labelled diagram in the report.

## Chapter 3 — Implemented Hardware System

- List all hardware actually used, including verified model numbers and roles. If a model, quantity, rating, or cost cannot be established, state that it is unverified rather than inventing a value.
- Resolve the saline servo discrepancy before submission: the actuator wiring figure says **SG90**, while firmware comments say **MG996R**. A generic servo description is appropriate until the physical model is confirmed.
- Describe the software environment: ESP-IDF, C/FreeRTOS firmware, PlatformIO/VS Code project settings where present, Firebase hosting/database/authentication, and HTML/CSS/JavaScript dashboard interfaces. Record versions only when the repository or verified build establishes them.
- Add **actual prototype photographs** with visible main-part labels. The template explicitly requires identifying the main parts rather than inserting an unannotated photo. Briefly explain every included photo. A full-system photograph plus selected sensor/mechanism/electronics close-ups is appropriate.
- Include sensor and actuator connection information. The supplied PNGs are wiring sketches; they do not replace a complete electrical schematic or prove power capacity/protection.
- Exclude the example `fig1.png`, which is a template graphic unrelated to the project.

## Chapter 4 — Results

The template requires **step-by-step evidence** sufficient to assess each subsystem, analogous to: input reading in normal and triggered conditions, controller decision/signal, and final output activation. For this project, present available results for sensor/UI states, cloud telemetry, configured thresholds, and documented actuator behavior.

- Clearly label each evidence type: **photographed assembly**, **screenshot observation**, **firmware-derived behavior**, **host-side logic check**, or **physical measurement**. Reading code or simulating a condition does not prove physical actuator operation.
- The supplied dashboard/database snapshots report 324.1 mL, zero BPM and SpO2, an open saline valve, a standby syringe, and disconnected/zero ECG. Use these as interface and schema evidence. They do not validate vital-sign or volume accuracy.
- The source closes saline when the remaining-volume estimate is **strictly below** the active threshold. The visible 324.1 mL estimate with threshold 320 mL and Open state is therefore internally consistent.
- Show real sensor data for valid and invalid/no-contact conditions when such measurements become available. Supply actual below-threshold and above-threshold traces, decision logs, and output photographs/video before asserting successful end-to-end control.
- Response time and other attributes may be included if actually measured. Distinguish theoretical loop timing from measured hardware/cloud latency.
- Do not invent test participants, precision, clinical outcomes, dosage accuracy, savings, battery life, or measured costs.

## Chapter 5 — Discussion and P1–P7 mapping

The template asks for limitations, at least one failure case, conflicting requirements, two alternatives, impact, standards/ethics, stakeholder input, and module dependence. It includes a **Complex Engineering Problem Mapping** table with P1 mandatory and applicable P2–P7 checked. Explain every checked item in the discussion; do not check unsupported criteria merely to fill the table.

| Code | Template meaning | Required discussion/evidence |
|---|---|---|
| P1 | Depth of Knowledge | Explain knowledge needed across microcontroller interfaces, signals, control logic, cloud/web integration, and the physical mechanism. P1 is required in the mapping table. |
| P2 | Conflicting Requirements | Write 2–3 sentences on actual cost/performance, speed/smoothing, power/portability, or convenience/control tradeoffs where relevant. Identify how the chosen design resolves them. |
| P3 | Depth of Analysis | Describe **two alternative designs** and explain differences in budget, performance, complexity, or suitability. Make a substantive comparison with the implemented design. |
| P4 | Familiarity of Issues | Explain impact and the need to engage with a field outside routine CSE work, such as medicine, physical delivery mechanisms, and physiological measurement. Separate expected benefit from evaluated impact. |
| P5 | Extent of Applicable Codes | In 2–3 sentences where applicable, identify standards/manuals/interface documents studied, adherence or deviations, and ethical considerations. Do not claim regulatory or medical-device compliance merely because datasheets were used. |
| P6 | Extent of Stakeholders | State whether users, managers, funders, clinicians, or other stakeholders were actually consulted, what they requested, and how design changed. No consultation records are supplied; leave this limitation explicit and do not invent interviews. |
| P7 | Interdependence | In 2–3 sentences, explain module coupling, dependencies, and components communicating across devices. Discuss the sensor/control task, shared state, network task, database schema, and browser control dependence where supported. |

Concrete failure/limitation cases should include uncalibrated load-cell conversion, invalid/no-finger vital readings, disconnected ECG, network errors/stale commands, servo/mechanism uncertainties, missing delivered-dose verification, and unverified secondary mobility firmware where applicable. Describe actual mechanisms and consequences, without claiming observed failures that have not been tested.

## Chapter 6 — Conclusion

- Summarize implemented project features in **1–2 sentences**.
- Discuss demonstrated efficiency/evidence and future scope in **3–4 sentences**.
- Keep claims proportional to provided evidence. Future work should include calibration, independent sensor/reference comparisons, measured delivery, robust command/fault handling, and refinement of unsupported concept features. SpO2, ECG and Firebase already exist in source/UI, so do not describe their initial addition as future work merely because an older slide does.

## Acknowledgment

Use an unnumbered acknowledgment section. Mention people who helped, borrowed lab/friend equipment, and sponsors only when the team confirms them. The template prefers the spelling **Acknowledgment**, without an extra `e` after `g`. Avoid the sample phrase “one of us ... thanks.” Confirmed sponsor acknowledgment belongs in the first-page unnumbered footnote. Missing helper/lab/funding details should remain a transparent placeholder or an explicit statement that no details were supplied.

The current acknowledgment names the user-confirmed supervisor, **Arnab Bhattacharjee**. Further laboratory, borrowed-equipment, helper, or funding acknowledgments should be added only when confirmed.

## References

- The original template explicitly requires the **GitHub repository link and YouTube demonstration link** in the references. The GitHub reference is now filled with the user-supplied [project repository](https://github.com/khalidhasan7343/CSE_4326_-Microprosser---Micro-controller-_Project). The **YouTube demonstration URL remains pending** because it has not been supplied; the prior authorization to retain a missing-URL placeholder continues to apply. Replace that placeholder before submission; do not invent a link or upload/publish material.
- Remove unrelated example bibliography entries. Cite relevant literature, original technical manuals/datasheets, and official software/interface documentation.
- Number citations consecutively in square brackets; place sentence punctuation after the citation, following the IEEE template. Use the reference number directly rather than “Ref.”/“reference,” except the template allows “Reference [number]” at the beginning of a sentence.
- Number footnotes separately as superscripts, place footnote text at the foot of the citing column, and avoid footnotes in the abstract or reference list. Use letters for table footnotes.
- List all authors when there are fewer than six; do not use `et al.` in that case. Describe unpublished work as “unpublished” and accepted work as “in press.” Capitalize only the first word of paper titles apart from proper nouns and element symbols. For translated journals, give the English citation before the original-language citation.

## Figure, table and notation requirements

- Follow the supplied LaTeX figure/table syntax and IEEE style. Prefer placement at the **top or bottom of a column**, avoiding the middle. A large item may span both columns.
- Put **figure captions below** figures and **table captions/headings above** tables. The original complex-problem sample places its caption below; the explicit formatting instruction takes precedence when preparing the final table.
- Cite an item in the prose before inserting it. Refer to figures as **Fig.** even at the beginning of a sentence. Give every figure/table a unique label and meaningful caption.
- The template specifies **8-point Times New Roman figure labels**. Use readable Times-style labels at an appropriate size in generated diagrams/annotations.
- Use words for axes, not only abbreviations/symbols. Include units in parentheses: e.g., **Remaining volume (mL)**, **Time (s)**, **Heart rate (beats/min)**. Do not label an axis only with its unit or write a quantity/unit ratio such as `Temperature/K`.
- Actual prototype photos should have part labels and captions that match visible parts. Preserve original photos and use separate annotated report copies when annotations are added.
- Avoid repeating duplicate screenshots: `web_6.jpeg` equals `web_1.jpeg`, and `web_7.jpeg` equals `web_2.jpeg`. The Firebase console screenshot includes account identifiers; prefer a schema drawing or cropped/anonymized material when illustrating the database.

## Details still needed before submission

- Report date/semester or any other additional administrative details if required by the course. Department, university, and supervisor are confirmed; city/country and email/ORCID are intentionally omitted at the user's request.
- Final YouTube demonstration URL. The GitHub repository URL is already supplied and filled.
- Verified saline-servo model and other obscured hardware models/ratings/quantities; supply/battery/regulator specifications; actual bill-of-materials prices if making a low-cost claim.
- Load-cell tare and known-weight calibration; bottle/tubing tare; documented mass-to-volume assumption and comparison with independent volume measurement.
- Reference-device comparisons for heart rate and SpO2, usable ECG traces, valid/invalid-contact tests, and nonclinical test conditions.
- Measured syringe displacement/delivered volume, saline cutoff behavior, actuator limits, alarm operation, and normal/triggered/fault evidence.
- Measured response time, network/cloud delay, disconnect behavior, battery runtime and mobility performance if these are claimed.
- Stakeholder consultation details and any design changes from their feedback, or an honest statement that consultation has not occurred.
- Any additional confirmed acknowledgment, lab equipment/borrowed-component or funding details beyond the supervisor acknowledgment already filled.

These missing items limit the strength of claims but do not prevent preparing a complete source-grounded report. The current draft should retain explicit placeholders and distinguish implementation evidence from the physical experiments still required.
