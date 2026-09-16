======================================================================================================================
# 🚀 Firmware Setup & Deployment Guide
======================================================================================================================

This guide provides step-by-step instructions for configuring, compiling, and flashing the firmware for both the self-balancing robot and its remote transmitter using **PlatformIO**.

======================================================================================================================
## 🛠️ Prerequisites & Installation
======================================================================================================================
Before flashing either device, ensure you have the following toolchain installed:
1. **VS Code** with the **PlatformIO IDE** extension installed.
2. An appropriate USB cable to connect your ESP32 boards directly to your computer.

======================================================================================================================
## 🎮 1. Remote Transmitter Firmware
======================================================================================================================
The remote controller firmware handles directional steering commands and pairs to the robot over ESP-NOW.

### ⚙️ Understanding `config.h` (Remote)
All transmitter configurations are managed centrally inside `firmware/remote_firmware/include/config.h`.

* **WIRELESS NETWORK [DO NOT MODIFY]:** Maps the primary Wi-Fi communication channel and broadcast pairing destination. Altering this breaks connection pairing with the robot.

* **SYSTEM TIMING:** Configures telemetry frequency. `SEND_INTERVAL_MS` handles continuous packet rates, while `CONNECTION_LED_TIMEOUT_MS` controls how long the pairing LED stays lit after a successful ACK packet response.

* **HARDWARE PIN MAPPING:** Defines the directional pushbuttons and system state feedback configurations.

### 🚀 How to Flash:
1. Open the **Master Root Repo** workspace folder in VS Code.
2. Open the **PlatformIO sidebar tool** (the alien head icon).
3. Locate and expand the **`env:remote_firmware`** environment under the Project Tasks list.
4. Connect your remote transmitter board via USB.
5. Click **Build**, followed by **Upload**.

======================================================================================================================
## 🤖 2. Robot Firmware & Global Configuration
======================================================================================================================
The robot firmware handles the core balancing logic, motor kinematics, audio playback, and system safety thresholds.

### ⚙️ Understanding `config.h` (Robot)
All system adjustments are managed centrally inside `firmware/robot_firmware/include/config.h`.

* **SYSTEM BOOT & TIMING [DO NOT MODIFY]:** Controls the core runtime math. `BOOT_TIME_MS` defines the initialization window before the robot engages. Altering loop intervals will disrupt hardware timers and destabilize the PID calculations.

* **HARDWARE PIN MAPPING:** Defines the layout for motor drivers, IMU I2C lines, and peripherals. *Warning: Do not alter if using my custom designed PCB.*

* **SAFETY THRESHOLDS:** Protects your hardware. `ACTIVATION_ANGLE` and `STOP_ANGLE` determine when motors engage or shut down to prevent runaway crashes, while `BATT_LOW_THRESHOLD` safeguards the 2S LiPo battery from over-discharging below a safe operating voltage.

* **PID LOOP TUNING:** Fine-tunes the balance responsiveness via Proportional (`KP`), Integral (`KI`), and Derivative (`KD`) gains.

* **MOTION & KINEMATICS:** Adjusts behavior when moving or executing turns, including motor bias adjustments (`LEFT_TRIM` / `RIGHT_TRIM`) to counteract minor mechanical drift.

* **AUDIO & LED CONFIGURATION:** Manages peripheral behaviors, capping volume ceilings and maximum LED brightness constraints for thermal protection.

### 🚀 How to Flash:
1. Open the **PlatformIO sidebar tool** in VS Code.
2. Locate and expand the **`env:robot_firmware`** environment.
3. Connect your self-balancing robot board via USB.
4. Click **Build**, followed by **Upload**.

======================================================================================================================
## 🎵 3. Custom Audio & Filesystem Image (LittleFS)
======================================================================================================================
The robot plays raw audio samples directly from internal flash storage using optimized timing. To prevent static distortion or incorrect pitch playback, follow these instructions precisely:

### 🎛️ Step 1: Prepare Your Audio Files
Custom audio clips **must** meet these strict hardware constraints:
* **Format:** `.wav` (Unsigned 8-bit PCM)
* **Channels:** Mono (1 channel)
* **Sample Rate:** 11,025 Hz *(Matches `SAMPLE_DELAY_MICROS = 91` in `config.h`)*

1. **Get Audio:** Download your own audio clip, or generate free text-to-speech voice lines using [VoiceGenerator.io](https://voicegenerator.io) or [ttsMP3.com](https://ttsmp3.com).
2. **Convert Audio:** Head to the [Online-Convert Audio to WAV](https://online-convert.com) tool.
3. **Configure Settings:** Under **Optional Settings**, apply these strict conversion rules:
   * Change bit depth: **`8 Bit`**
   * Change audio sample rate: **`11025 Hz`**
   * Change audio channels: **`Mono`**
4. **Download:** Click **Start**, then download your formatted `.wav` file.

### 📂 Step 2: Manage the Project Assets
PlatformIO expects a specific folder structure to compile binary filesystem data.
1. Place your converted `.wav` audio files directly inside the dedicated **`firmware/robot_firmware/data/`** directory.

### 🚀 Step 3: Flash the Filesystem
Your `platformio.ini` is pre-configured to handle the layout using the target flag `board_build.filesystem = littlefs`.

1. Open the **PlatformIO sidebar tool**.
2. Expand your **`env:robot_firmware`** environment entry.
3. Expand the **Platform** tasks dropdown list.
4. Click **Upload Filesystem Image**.

⚠️ **Important Note:** Flashing the filesystem will completely overwrite any files currently stored on the chip. Ensure all default files your robot's source code expects to find remain in the `data/` folder alongside your custom sounds before hitting upload.