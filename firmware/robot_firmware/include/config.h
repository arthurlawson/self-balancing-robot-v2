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
static constexpr float DEFAULT_SETPOINT = 1.3f; // Coded to be +1 = 1 degree towards the front
static constexpr float ACTIVATION_ANGLE = 3.0f;
static constexpr float STOP_ANGLE = 50.0f;

// ========== BASE PID TUNING ==========
static constexpr float KP = 30.0f;
static constexpr float KI = 0.0f;
static constexpr float KD = 0.05f;// 0.0
static constexpr float MIN_PWM = 30.0f;

// ========== FORWARD/BACKWARD ==========
static constexpr float MAX_DRIVE_LEAN_DEG = 5.0f;
static constexpr float MOVEMENT_RAMP_RATE = 0.01f;
static constexpr float BRAKING_RAMP_RATE = 0.15f;
static constexpr float TURN_SPEED_OFFSET = 30.0f;
static constexpr float LEFT_TRIM = 1.2f;
static constexpr float RIGHT_TRIM = 1.0f;