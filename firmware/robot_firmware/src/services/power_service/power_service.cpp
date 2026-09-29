#include <config.h>
#include "power_service.h"
#include "utils/time_utils.h"
#include "driver/gpio.h"

PowerService::PowerService(uint8_t battPin, LedService& ledSvc)
    : _battPin(battPin), _ledSvc(ledSvc), _curVoltage(0.0f), _isLowBatt(false), 
      _sagTimerStartMs(0), _lastSampleTimeMs(0) {

    _adc_channel = ADC1_CHANNEL_1;

}

void PowerService::Begin() {
    // Initialize the width of ADC Unit 1 to 12-bit (0-4095)
    adc1_config_width(ADC_WIDTH_BIT_12);

    // 12dB attenuation to read full ranges without clipping
    adc1_config_channel_atten(_adc_channel, ADC_ATTEN_DB_12);
}

bool PowerService::IsBatteryLow() {
    if (_isLowBatt) return true;

    uint32_t now = GetMillis();

    if (now - _lastSampleTimeMs >= TIME_BETWEEN_CHECKS || _lastSampleTimeMs == 0) {
        _lastSampleTimeMs = now;

        // Convert millivolts back to volts, and scale with the voltage divider ratio (3.0f)
        int rawReading = adc1_get_raw(_adc_channel);

        if (rawReading >= 0) {
            uint32_t voltageMilli = (rawReading * 3100) / 4095; // 3100mV max under 11dB atten

            _curVoltage = (static_cast<float>(voltageMilli) / 1000.0f) * 3.0f;

            // If voltage is below the threshold voltage for a set duration, the robot is permanantly latched off
            if (_curVoltage < BATT_LOW_THRESHOLD) {
                if (_sagTimerStartMs == 0) {
                    _sagTimerStartMs = now;
                } else if (now - _sagTimerStartMs > SAG_CONFIRMATION_DUR) {
                    _isLowBatt = true;
                    _ledSvc.SetState(LedService::LED_BLINK_SLOW);
                }
            } else {
                _sagTimerStartMs = 0;
            }
        }
    }
    
    return _isLowBatt;
}