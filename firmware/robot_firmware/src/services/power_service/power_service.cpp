#include "power_service.h"

PowerService::PowerService(uint8_t battPin, LedService& ledSvc, float threshold)
    : _battPin(battPin), _ledSvc(ledSvc), _threshold(threshold), _curVoltage(0.0f), _isLowBatt(false), _sagTimerStartMs(0) {}

void PowerService::Begin() {
    pinMode(_battPin, INPUT);
}

bool PowerService::IsBatteryLow() {
    if (_isLowBatt) return true;

    // Convert millivolts back to volts, and scale with the voltage divider ratio (3.0f)
    uint32_t pinMilliVolts = analogReadMilliVolts(_battPin);
    _curVoltage = (static_cast<float>(pinMilliVolts) / 1000.0f) * 3.0f;

    // If voltage is below the threshold voltage for a set duration, the robot is permanantly latched off
    if (_curVoltage < _threshold) {
        if (_sagTimerStartMs == 0) {
            _sagTimerStartMs = millis();
        } else if (millis() - _sagTimerStartMs > SAG_CONFIRMATION_DUR) {
            _isLowBatt = true;
            _ledSvc.SetState(LedService::LED_BLINK_SLOW);
        }
    } else {
        _sagTimerStartMs = 0;
    }

    return _isLowBatt;
}