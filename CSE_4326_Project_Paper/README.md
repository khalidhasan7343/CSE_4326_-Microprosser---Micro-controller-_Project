# MediSmart project report

The report follows the six sections required by the supplied IEEE conference template. Each chapter is a separate file, included by [conference_101719.tex](conference_101719.tex).

| Chapter | Editable source |
|---|---|
| 1. Introduction: problem, literature, contributions | [01_introduction.tex](chapters/01_introduction.tex) |
| 2. Proposed Method: block diagram, sensing, control, cloud | [02_proposed_method.tex](chapters/02_proposed_method.tex) |
| 3. Implemented Hardware System: inventory, wiring, environment, labeled photos | [03_implemented_hardware.tex](chapters/03_implemented_hardware.tex) |
| 4. Results: actual captures, source-derived cases, timing calculations, validation plan | [04_results.tex](chapters/04_results.tex) |
| 5. Discussion: limitations, alternatives, P1–P7 mapping | [05_discussion.tex](chapters/05_discussion.tex) |
| 6. Conclusion | [06_conclusion.tex](chapters/06_conclusion.tex) |
| Appendices: setup, operating instructions, artifact traceability | [07_appendices.tex](chapters/07_appendices.tex) |

Read [REPORT_INSTRUCTIONS.md](REPORT_INSTRUCTIONS.md) for the original assignment requirements and submission checklist. References are in [references.tex](references.tex). The original template is preserved in [materials/original_report_template.tex](materials/original_report_template.tex), and source-review notes are under `materials/`.

## Build and read

From this directory, run:

```powershell
latexmk -pdf -interaction=nonstopmode -halt-on-error conference_101719.tex
```

The output is [conference_101719.pdf](conference_101719.pdf). The build uses the included `IEEEtran.cls` and installed TeX packages including TikZ, graphicx, amsmath, booktabs, tabularx, cite, and hyperref. Keep the parent `Project_images`, `Website_images`, and `medismart` directories alongside this report directory because the report references the original photographs. Photo annotations and the block diagram are vector elements in LaTeX; the original images are preserved.

## Confirmed metadata and remaining details

The confirmed department is **BSCSE**, the university is **United International University**, and the supervisor is **Arnab Bhattacharjee**. The acknowledgment names the supervisor. The GitHub reference is filled with the supplied [project repository](https://github.com/khalidhasan7343/CSE_4326_-Microprosser---Micro-controller-_Project). The city/country and email/ORCID lines are omitted at the user's explicit request, overriding those fields in the original template.

The **YouTube demonstration URL is still pending** because it has not been provided. Its placeholder remains in [project_metadata.tex](project_metadata.tex), as previously authorized. Replace it before submission. Verify the author spellings and student IDs against the original slide deck, and add any further confirmed acknowledgment or funding details only if applicable.

The results chapter states what the supplied materials establish. The screenshots contain zero physiological readings and a displayed 324.1 mL fluid estimate; no calibrated trial dataset is supplied. Measured sensor accuracy, actual delivered volume, actuator/alarm response, cloud delay, cost, and power efficiency need real experimental records before quantitative claims can be added. The proposed validation table identifies the measurements to collect.

The hardware chapter records the SG90/MG996R saline-servo discrepancy, missing component ratings, and 4 MB/2 MB flash configuration mismatch. The discussion records schedule repetition, reset behavior, sensor availability, chart limitations, and authentication/TLS gaps. Update these descriptions if the implementation is corrected and new evidence becomes available.

Firmware and website application files have not been changed by report generation.
