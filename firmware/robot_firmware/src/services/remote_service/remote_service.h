/**
 * @file remote_service.h
 * @brief Service for handling remote control commands via ESP-NOW on the ESP32-S3 Self-Balancing Robot.
 */

#pragma once

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "protocol.h"
#include "system/robot_controller/robot_controller.h"

class RemoteService {
public:
    static void Begin(RobotController& robot);
    static void Update(); // Handle safety watchdog

private:
    static void OnDataReceived(const uint8_t* mac, const uint8_t* data, int len);

    static RobotController* _robot;
    static unsigned long _lastPacketTime;
    static const unsigned long WATCHDOG_TIMOUT_MS = 100; // 0.1s
    
};