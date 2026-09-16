/**
 * @file config.h
 * @brief Global configuration constants for the remote transmitter.
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>

// =====================================================================================
// ESP-NOW WIRELESS NETWORK CONFIGURATION
// =====================================================================================
// *** WARNING: Changing these values will disrupt wireless pairing with the robot.

// Broadcast MAC Address to target all local devices (FF:FF:FF:FF:FF:FF)
static const uint8_t BROADCAST_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Primary Wi-Fi communication channel (Must match the robot's channel exactly)
static constexpr uint8_t WIFI_CHANNEL = 1;

// =====================================================================================
// SYSTEM TIMING
// =====================================================================================
// *** Adjust these intervals to tweak transmission frequency and telemetry feedback.

// ESP-NOW data transmission interval (50ms is half of the robot firmware watchdog timeout)
static constexpr uint32_t SEND_INTERVAL_MS = 50;

// Maximum time duration to keep the connection LED lit following a successful ACK packet
static constexpr uint32_t CONNECTION_LED_TIMEOUT_MS = 200;

// =====================================================================================
// HARDWARE PIN MAPPING
// =====================================================================================
// *** Update these pins to match your physical remote transmitter hardware layout.

// Directional inputs (Active-Low Buttons with internal pullups)
static constexpr uint8_t FWD_PIN = 3;
static constexpr uint8_t LFT_PIN = 4;
static constexpr uint8_t RGT_PIN = 6;
static constexpr uint8_t BWD_PIN = 10;

// Status feedback indicator
static constexpr uint8_t LED_PIN = 5;