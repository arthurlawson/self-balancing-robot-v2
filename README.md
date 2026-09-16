============================================================================================
# 🤖 Self-Balancing Robot V2
============================================================================================

### A Drivable Self-Balancing Robot That Screams When It Falls!

An advanced self-balancing robot utilizing the **ESP32-S3 (N16R8)** computing core, driven by a custom **Kalman Filter** and high-frequency **PID control loop** system.

This project features a fully **custom-designed main controller PCB, proudly sponsored and manufactured by JLCPCB**.

Built entirely within the PlatformIO ecosystem, this hardware platform integrates an MPU6050 IMU, DRV8833 motor driver, 2S LiPo battery safety logic, lighting, and onboard speaker playback systems.

============================================================================================
## Core Features
============================================================================================

*   **Advanced Estimation:** Custom Kalman Filter implementation paired with an MPU6050 IMU for precise roll angle determination.
*   **Wireless Remote Control:** Directional steering control handled over a dedicated **ESP-NOW** wireless connection.
*   **Motion:** Dual-motor balancing algorithms with custom gain scheduling to aid in active control.
*   **Hardware Protection:** Automated motor shutdown constraints to prevent destructive runaway crashes.
*   **Power Management:** Active voltage checks safeguarding the 2S LiPo system from dropping beneath unsafe cell margins.
*   **Audio & Visuals:** Integrated 2P edison filament LED's with status animations alongside audio playback to make the robot scream.

============================================================================================
## Quick Navigation Hub
============================================================================================

To get started with assembling or programming the robot, navigate to the specific setup guides below:

*   **[Firmware Setup & Deployment](./firmware/SETUP_GUIDE.md):** Complete code overview, configuration walkthrough for `config.h`, custom audio formatting parameters, and PlatformIO flashing instructions.
*   **[Hardware Production & Assembly](./hardware/SETUP_GUIDE.md):** Access to print-ready `.3mf` files, raw SolidWorks CAD models, custom KiCad schematics, and factory-ready Gerber packages.
*   **[Bill of Materials (BOM)](./hardware/BOM.md):** The complete physical and electrical component checklist required to buy, print, and manufacture the robot.

============================================================================================
## Project License
============================================================================================

This repository is distributed under a custom **Non-Commercial Open-Source License**. 

*   **Personal & Educational Use:** Freely allowed to modify, merge, and run the code/hardware assets.
*   **Commercial Use:** Strictly prohibited from being sold, leased, or utilized for paid product development without explicit written consent from the copyright holder.

*See the full terms inside the accompanying [LICENSE](./LICENSE) file.*