/**
 * @file led_service.h
 * @brief Non blocking PWM LED service with set states for the lights
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>
#include "driver/ledc.h"

class LedService {
    public:
        enum LedState {
            LED_BOOT_UP,   // Smooth fade from 0 to max brightness over 1 second when booting up
            LED_ON,        // Solid max brightness when active
            LED_PULSING,   // Continuous breathing fading for when fallen over
            LED_BLINK_SLOW // 1Hz flash when critical low battery
        };
        LedService(uint8_t ledPin);
        void Begin();
        void Update();
        void SetState(LedState state);
        bool IsBootComplete() const { return _bootComplete; }
    private:
        uint8_t _ledPin;
        LedState _curState;
        bool _blinkState;
        bool _bootComplete;

        unsigned long _lastToggleTime;
        unsigned long _stateStartTime;

        static constexpr uint16_t BOOT_TIME_MS = 1500;
        static constexpr uint8_t MAX_BRIGHT = 225; // Thermal protection hence not using max 255
        static constexpr uint16_t BLINK_INTERVAL_MS = 1000;

        // Hardware segregation - bound to channel 1 and timer 1
        static constexpr ledc_channel_t LED_CH = LEDC_CHANNEL_1;
        static constexpr ledc_timer_t LED_TIMER = LEDC_TIMER_1;

        void WriteBrightness(uint32_t duty) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CH, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CH);
        }
};
