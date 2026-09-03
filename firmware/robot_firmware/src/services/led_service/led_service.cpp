#include "led_service.h"

LedService::LedService(uint8_t ledPin)
    : _ledPin(ledPin), _curState(LED_BOOT_UP), _blinkState(false), _bootComplete(false), _lastToggleTime(0), _stateStartTime(0) {}

void LedService::Begin() {
    _stateStartTime = millis();
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT, 
        .timer_num        = LED_TIMER,   
        .freq_hz          = 20000,            
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = _ledPin,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LED_CH,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LED_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void LedService::Update() {
    unsigned long now = millis();

    switch (_curState) {
        case LED_BOOT_UP: {
            unsigned long elapsed = now - _stateStartTime;
            if (elapsed >= BOOT_TIME_MS) {
                WriteBrightness(MAX_BRIGHT);
                _bootComplete = true;
                SetState(LED_ON);
            } else {
                uint8_t bootBrightness = (elapsed * MAX_BRIGHT) / BOOT_TIME_MS;
                WriteBrightness(bootBrightness);
            }
            break;
        }
        case LED_ON: {
            break; 
        }
        case LED_PULSING: {
            float angle = (float)(now - _stateStartTime) * (3.14159f / 1000.0f);
            uint8_t pulseBrightness = (uint8_t)((sin(angle) + 1.0f) * (MAX_BRIGHT / 2));
            WriteBrightness(pulseBrightness);
            break;
        }
        case LED_BLINK_SLOW: {
            if (now - _lastToggleTime >= BLINK_INTERVAL_MS) {
                _blinkState = !_blinkState;
                WriteBrightness(_blinkState ? MAX_BRIGHT : 0);
                _lastToggleTime = now;
            }
            break;
        }
        default:
            break;
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
            WriteBrightness(0);
            break;
        case LED_ON:
            WriteBrightness(MAX_BRIGHT);
            break;
        case LED_PULSING:
            break;
        case LED_BLINK_SLOW:
            _blinkState = true;
            WriteBrightness(MAX_BRIGHT);
            _lastToggleTime = now;
            break;
    }
}