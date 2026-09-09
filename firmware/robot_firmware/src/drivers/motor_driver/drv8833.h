/**
 * @file drv8833.h
 * @brief DRV8833 motor driver class for controlling two DC motors.
 * 
 * @author Arthur Lawson
 * 
 */

#pragma once

#include <Arduino.h> 
#include "driver/ledc.h"

class DRV8833 {
    public: 
        DRV8833(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);
        void Drive(int16_t left, int16_t right);
        void StopBoth();

    private: 
        uint8_t _in1, _in2, _in3, _in4;

        static constexpr ledc_channel_t IN1_CH = LEDC_CHANNEL_2;
        static constexpr ledc_channel_t IN2_CH = LEDC_CHANNEL_3;
        static constexpr ledc_channel_t IN3_CH = LEDC_CHANNEL_4;
        static constexpr ledc_channel_t IN4_CH = LEDC_CHANNEL_5;
        static constexpr ledc_timer_t MOTOR_TIMER = LEDC_TIMER_2;

        void SetPWM(ledc_channel_t channel, uint32_t duty) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
        }
        
};
