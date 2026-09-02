/**
 * @file config.h
 * @brief Global configuration constants for the robot.
 */

#pragma once

#include <Arduino.h>

// ========== TIMING ==========
static constexpr float DELTA_T_MS = 5.0f;
static constexpr float DELTA_T_SEC = DELTA_T_MS / 1000.0f;
static constexpr uint32_t TIMER_ALARM_US = static_cast<uint32_t>(DELTA_T_MS * 1000.0f);
static constexpr uint16_t TIMER_PRESCALER = 80; // 80MHz / 80 = 1 tick per microsecond

// ========== HARDWARE PINS ==========
static constexpr uint8_t BATT_PIN = 2;
static constexpr uint8_t IN1_PIN = 4;
static constexpr uint8_t IN2_PIN = 5;
static constexpr uint8_t IN3_PIN = 6;
static constexpr uint8_t IN4_PIN = 7;
static constexpr uint8_t SCL_PIN = 11;
static constexpr uint8_t SDA_PIN = 12;
static constexpr uint8_t SPEAKER_PIN = 13;
static constexpr uint8_t LED_PIN = 39;

// ========== SAFETY & LIMITS ==========
static constexpr float BATT_THRESHOLD = 5.5f;
static constexpr float DEFAULT_SETPOINT = 0.7f;
static constexpr float ACTIVATION_ANGLE = 3.0f;
static constexpr float STOP_ANGLE = 80.0f;

// ========== BASE PID TUNING ==========
static constexpr float KP = 25.0f;
static constexpr float KI = 130.0f;
static constexpr float KD = 0.7f;
static constexpr float MIN_PWM = 20.0f;

// ========== IDLE ==========
static constexpr float START_EXPONENTIAL = 2.0f;

// ========== FORWARD/BACKWARD ==========
static constexpr float DRIVE_BASE_LEAN = 1.15f;
static constexpr float LEFT_TRIM = 1.0f;
static constexpr float RIGHT_TRIM = 1.10f;