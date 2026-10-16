# Verified primary bibliographic sources

Verification date: 8 October 2026. Publisher and standards-owner sources were used. Some publisher page opens were rate-limited; indexed publisher article/reprint metadata still exposed the details below.

## Applied Sciences paper

Jae-Kyeong Lee, Ki-Cheol Yoon, and Kwang Gi Kim, "Design of a Remote Monitoring System Based on Optical Sensors to Prevent Medical Accidents during Fluid Treatment," *Applied Sciences*, vol. 11, no. 21, article 10124, 2021, doi: 10.3390/app112110124. Published 28 October 2021; received 19 August, revised 12 October and accepted 25 October 2021. [MDPI article](https://www.mdpi.com/2076-3417/11/21/10124).

The paper combines optical drop sensing with load-cell measurements and remote monitoring. This is relevant prior work, not evidence of this project's measured performance.

## Engineering Proceedings paper

Uriel Abe Contardi, Mateus Morikawa, Bruno Brunelli, and Douglas Vieira Thomaz, "MAX30102 Photometric Biosensor Coupled to ESP32-Webserver Capabilities for Continuous Point of Care Oxygen Saturation and Heartrate Monitoring," *Engineering Proceedings*, vol. 16, no. 1, article 9, **2022**, doi: 10.3390/IECB2022-11114. [MDPI article](https://www.mdpi.com/2673-4591/16/1/9?type=check_update&version=1).

Publisher metadata has a date inconsistency: the article and [SciForum conference record](https://sciforum.net/paper/11114) display 14 October 2021 as the publication date, while the formal citation is 2022 and the conference was 14-18 February 2022. [Version notes](https://www.mdpi.com/2673-4591/16/1/9/notes) list initial HTML/PDF uploads on 8 April 2022. Retain 2022 in the bibliography and avoid an unsupported exact-day claim.

This paper supports the design precedent for coupling MAX30102 sensing with ESP32 web monitoring. Its results must not be transferred to this project's custom sensing code.

## Electronics paper

Chiang Liang Kok, Chee Kit Ho, Teck Kheng Lee, Zheng Yuan Loo, Yit Yan Koh, and Jian Ping Chai, "A Novel and Low-Cost Cloud-Enabled IoT Integration for Sustainable Remote Intravenous Therapy Management," *Electronics*, vol. 13, no. 10, article 1801, 2024, doi: 10.3390/electronics13101801. Published 7 May 2024; received 28 March, revised 22 April and accepted 30 April 2024. [MDPI article](https://www.mdpi.com/2079-9292/13/10/1801/html), [publisher author list](https://www.mdpi.com/2079-9292/13/10/1801/reprints).

This is prior work on ESP32/RTOS/cloud infusion management; its architecture uses Blynk. Describe this project's Firebase approach from local source code.

## I2C specification

NXP Semiconductors, *I2C-bus specification and user manual*, UM10204, revision 7.0, 1 October 2021. [Official PDF](https://www.nxp.com/docs/en/user-guide/UM10204.pdf).

The manual specifies SDA/SCL signaling, addressing, acknowledgement, timing and electrical constraints; 100 kbit/s is Standard-mode and 400 kbit/s is Fast-mode. It is a relevant interface reference, not certification of the assembled prototype. Use controller/target terminology, as revised in this edition.

## ACM professional code

Association for Computing Machinery, *ACM Code of Ethics and Professional Conduct*, adopted 22 June 2018. [Official booklet](https://acm.org/binaries/content/assets/about/acm-code-of-ethics-booklet.pdf), [official code text](https://acm.org/binaries/content/assets/about/acm-code-of-ethics-and-professional-conduct.pdf).

Relevant principles: 1.2 avoiding harm; 1.3 honest claims; 1.6 privacy; 2.5 credible system/risk evaluation; 2.9 security and usability. Direct PDF opens were forbidden, but official publisher search extracts verified these sections and the adoption date. The project connection is an engineering inference: honestly disclose idle screenshots and sensing limitations, restrict access to telemetry/control, and avoid implying validated clinical operation.

## Infusion safety guidance

U.S. Food and Drug Administration, *Infusion Pumps Total Product Life Cycle: Guidance for Industry and FDA Staff*, issued 2 December 2014. [Official guidance page](https://www.fda.gov/regulatory-information/search-fda-guidance-documents/infusion-pumps-total-product-life-cycle), [official PDF](https://www.fda.gov/media/78369/download).

The guidance discusses hazard analysis, performance evidence, alarm false positives/negatives, safety controls, wireless/security hazards and usable instructions. Sections 5B-5F are useful as a design-review comparator. It is U.S. nonbinding guidance and does not establish prototype approval or local legal compliance. The current PDF includes a cover note pointing to June 2023 software-documentation guidance; do not cite the old Major Level of Concern framework as current terminology.
