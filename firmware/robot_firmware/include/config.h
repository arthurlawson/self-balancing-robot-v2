/**
 * @file config.h
 * @brief Global configuration constants for the robot.
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>

// =====================================================================================
// SYSTEM BOOT
// =====================================================================================
// *** Adjust as wanted

// Global initialization sequence duration before the robot begins balancing (in milliseconds)
static constexpr uint16_t BOOT_TIME_MS = 1500;

// =====================================================================================
// SYSTEM TIMING
// =====================================================================================
// *** WARNING: Changing loop timing will disrupt the hardware timers and destabilize the PID loop.

// Loop update interval in milliseconds
static constexpr float DELTA_T_MS = 5.0f;

// Loop update interval in seconds
static constexpr float DELTA_T_SEC = DELTA_T_MS / 1000.0f;

// Hardware timer alarm interval in microseconds
static constexpr uint32_t TIMER_ALARM_US = static_cast<uint32_t>(DELTA_T_MS * 1000.0f);

// Timer prescaler clock divider (80MHz / 80 = 1 tick per microsecond)
static constexpr uint16_t TIMER_PRESCALER = 80;

// =====================================================================================
// HARDWARE PIN MAPPING
// =====================================================================================
// *** Adjust these pins to match your physical hardware wiring layout
// *** WARNING: If you use the PCB that I designed, do not change these

// Power monitoring
static constexpr uint8_t BATT_PIN = 2;

// Motor driver (H-Bridge)
static constexpr uint8_t IN1_PIN = 4;
static constexpr uint8_t IN2_PIN = 5;
static constexpr uint8_t IN3_PIN = 6;
static constexpr uint8_t IN4_PIN = 7;

// I2C Communication (IMU)
static constexpr uint8_t SCL_PIN = 11;
static constexpr uint8_t SDA_PIN = 12;

// Peripherals
static constexpr uint8_t SPEAKER_PIN = 13;
static constexpr uint8_t LED_PIN = 39;

// =====================================================================================
// SAFETY THRESHOLDS
// =====================================================================================
// *** Adjust these parameters to safeguard your robot during testing

// Angle in degrees to where the motors engage to start balancing
static constexpr float ACTIVATION_ANGLE = 3.0f;

// Angle in degrees where the motors shut off to prevent runaway crashes 
static constexpr float STOP_ANGLE = 50.0f;

// At this battery level for the 2S lipo of 8.4V, the robot will lock itself off (5.5V is too low for good motor function)
static constexpr float BATT_LOW_THRESHOLD = 5.5f;

// =====================================================================================
// PID CONTROL LOOP TUNING
// =====================================================================================
// *** Adjust these parameters to stabilize your robot's balance

// Target balance pitch angle in degrees (accounts for center of mass offset)
static constexpr float DEFAULT_SETPOINT = 1.3f; 

// Proportional gain
static constexpr float KP = 30.0f;

// Integral gain
static constexpr float KI = 0.0f;

// Derivative Gain
static constexpr float KD = 0.05f;

// Minimum PWM to overcome static motor friction
static constexpr float MIN_PWM = 30.0f;

// =====================================================================================
// MOTION AND KINEMATICS
// =====================================================================================
// *** Adjust these values to change how the robot behaves when moving or turning

// The driving lean angle
static constexpr float MAX_DRIVE_LEAN_DEG = 2.0f;

// Acceleration rate for speed transitions
static constexpr float MOVEMENT_RAMP_RATE = 0.01f;

// Deceleration rate when shifting directions quickly
static constexpr float BRAKING_RAMP_RATE = 0.15f;

// PWM offset injected to execute turns
static constexpr float TURN_SPEED_OFFSET = 30.0f;

// Motor bias compensation that scales each motor strength (In this case the left motor gets a 10% boost)
static constexpr float LEFT_TRIM = 1.1f;
static constexpr float RIGHT_TRIM = 1.0f;

// =====================================================================================
// AUDIO CONFIGURATION
// =====================================================================================
// *** Adjust the sample rates, volume ceilings and playback rules

// Global volume limit ceiling (Value ranges from 0 to 255)
static constexpr uint32_t MAX_VOLUME = 180;

// Minimum time required between consecutive playbacks (in milliseconds)
static constexpr uint32_t MIN_BETWEEN_PLAYS_MILLIS = 5000;

// Maximum allowed duration for a single audio clip
static constexpr uint32_t MAX_PLAY_DURATION_MILLIS = 3000;

// Step interval between each audio sample (91us for the 11025 Hz sample rate)
static constexpr uint32_t SAMPLE_DELAY_MICROS = 91;

// =====================================================================================
// LED CONFIGURATION
// =====================================================================================
// *** Adjust the LED brightness, flash rates, and pulse speeds

// Maximum allowed brightness level (Value ranges from 0 to 255)
static constexpr uint8_t MAX_BRIGHT = 50;

// Active duration gap between toggles when utilizing the slow blink pattern
static constexpr uint16_t BLINK_INTERVAL_MS = 1000;

// Pulse speed animation multiplier (Higher numbers yield slower pulsing rates)
static constexpr float PULSE_SPEED = 1000.0f;