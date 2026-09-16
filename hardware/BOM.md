============================================================================================
# Bill of Materials (BOM) — Self-Balancing Robot V2
============================================================================================

This document tracks all electronic, drivetrain, mechanical, and structural components required to fully manufacture and assemble the robot.

============================================================================================
## 1. Core Electronics & Custom PCB Components
============================================================================================

These components populate the custom controller PCB fabrications:
*   **Microcontroller:** ESP32-S3 (N16R8 Development Board variant)
*   **Inertial Measurement Unit (IMU):** MPU6050 Module (Breakout board style)
*   **Motor Driver Module:** DRV8833 Dual H-Bridge Motor Driver
*   **Power Regulation Module:** MPM3610 3.3V Buck Regulator (Highly efficient synchronous step-down)
*   **Peripherals:** Onboard 4-ohm Speaker, a 2P edison filament light setup connected to the top shelf
*   **Passives Checklist:** Assorted resistors (including voltage dividers for battery sensing tracking lines), capacitors, tactile buttons, and male/female header pins

============================================================================================
## 2. Drivetrain & Power Systems
============================================================================================

*   **Motors:** 2x TT Dual-Shaft DC Gearbox Motors (Yellow hobby style motors)
*   **Power Source:** 2S LiPo Battery (7.4V nominal, 8.4V max)
*   **Switching:** Main power toggle switch

============================================================================================
## 3. Mechanical Fasteners
============================================================================================

*   **Chassis Assembly:** M3 Hex Screws (Assorted lengths) and nuts, and M3 brass heat set inserts
*   **Motor Mount Fasteners:** M3 Screws and nuts compatible with standard TT motor chassis slots

============================================================================================
## 4. Fabrication Components & Materials
============================================================================================

*   **Custom PCB Manufacturing (`/electronics/fabrication`):** Main trace layout designed for manufacturing directly by **JLCPCB** using the provided `gerbers.zip` archive.
*   **Structural Rigid Printing Parts (`/cad/3d_prints`):** Printed entirely in **PLA+** for impact protection to withstand crashes when the robot falls over and screams.
    *   *Includes components:* BackPiece, FrontFacePiece, InnerChassis, LightDiffuser, MiddleHeadCover, TopShelf, and WheelRim.
*   **Traction Components (`/cad/3d_prints`):** `WheelTread.3MF` **must be manufactured in TPU** by **JLCPCB** to guarantee better balancing traction.