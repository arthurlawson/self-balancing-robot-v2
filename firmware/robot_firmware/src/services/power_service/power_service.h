/**
 * @file power_service.h
 * @brief Service to manage battery voltage monitoring and safety shutdown at low voltage levels
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>
#include "services/led_service/led_service.h"

class PowerService {
    public:
        PowerService(uint8_t battPin, LedService& ledSvc, float threshold);
        void Begin();
        bool IsBatteryLow();
    private:
        LedService& _ledSvc;
        uint8_t _battPin;
        float _threshold;
        float _curVoltage;

        bool _isLowBatt;
        unsigned long _sagTimerStartMs;
        static constexpr unsigned long SAG_CONFIRMATION_DUR = 1000;
};