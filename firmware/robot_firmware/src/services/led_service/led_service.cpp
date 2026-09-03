#include "led_service.h"

LedService::LedService(uint8_t ledPin)
    : _ledPin(ledPin), _curState(LED_BOOT_UP), _blinkState(false), _bootComplete(false), _lastToggleTime(0), _stateStartTime(0) {}

void LedService::Begin() {
    _stateStartTime = millis();
    analogWrite(_ledPin, 0);
}

void LedService::Update() {
    unsigned long now = millis();

    switch (_curState) {
        case LED_BOOT_UP: {
            unsigned long elapsed = now - _stateStartTime;
            if (elapsed >= BOOT_TIME_MS) {
                analogWrite(_ledPin, MAX_BRIGHT);
                _bootComplete = true;
                SetState(LED_ON);
            } else {
                uint8_t bootBrightness = (elapsed * MAX_BRIGHT) / BOOT_TIME_MS;
                analogWrite(_ledPin, bootBrightness);
            }
            break;
        }
        case LED_ON: {
            break; 
        }
        case LED_PULSING: {
            float angle = (float)(now - _stateStartTime) * (3.14159f / 1000.0f);
            uint8_t pulseBrightness = (uint8_t)((sin(angle) + 1.0f) * (MAX_BRIGHT / 2));
            analogWrite(_ledPin, pulseBrightness);
            break;
        }
        case LED_BLINK_SLOW: {
            if (now - _lastToggleTime >= BLINK_INTERVAL_MS) {
                _blinkState = !_blinkState;
                analogWrite(_ledPin, _blinkState ? MAX_BRIGHT : 0);
                _lastToggleTime = now;
            }
            break;
        default:
            break;
        }
    }
}

void LedService::SetState(LedState state) {
    if (_curState == state) return;
    _curState = state;

    unsigned long now = millis();
    _stateStartTime = now;

    switch (state) {
        case LED_BOOT_UP:
            _bootComplete = false;
            analogWrite(_ledPin, 0);
            break;
        case LED_ON:
            analogWrite(_ledPin, MAX_BRIGHT);
            break;
        case LED_PULSING:
            break;
        case LED_BLINK_SLOW:
            _blinkState = true;
            analogWrite(_ledPin, MAX_BRIGHT);
            _lastToggleTime = now;
            break;
    }
}