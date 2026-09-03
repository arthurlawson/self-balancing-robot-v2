#include "drv8833.h"

DRV8833::DRV8833(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4) 
    : _in1(in1), _in2(in2), _in3(in3), _in4(in4) {
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT, 
        .timer_num        = MOTOR_TIMER,   
        .freq_hz          = 20000,            
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ch_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = MOTOR_TIMER,
        .duty       = 255, 
        .hpoint     = 0
    };

    ch_config.gpio_num = _in1; ch_config.channel = IN1_CH; ledc_channel_config(&ch_config);
    ch_config.gpio_num = _in2; ch_config.channel = IN2_CH; ledc_channel_config(&ch_config);
    ch_config.gpio_num = _in3; ch_config.channel = IN3_CH; ledc_channel_config(&ch_config);
    ch_config.gpio_num = _in4; ch_config.channel = IN4_CH; ledc_channel_config(&ch_config);

}

void DRV8833::Drive(int16_t left, int16_t right) {

    left = constrain(left, -255, 255);
    if (left > 0) {
        SetPWM(IN1_CH, 255 - left);
        SetPWM(IN2_CH, 255);
    } else if (left < 0) {
        SetPWM(IN1_CH, 255);
        SetPWM(IN2_CH, 255 + left);
    } else {
        SetPWM(IN1_CH, 255);
        SetPWM(IN2_CH, 255);
    }

    right = constrain(right, -255, 255);
    if (right > 0) {
        SetPWM(IN3_CH, 255);
        SetPWM(IN4_CH, 255 - right);
    } else if (right < 0) {
        SetPWM(IN3_CH, 255 + right);
        SetPWM(IN4_CH, 255);
    } else {
        SetPWM(IN3_CH, 255);
        SetPWM(IN4_CH, 255);
    }
}

void DRV8833::StopBoth() {
    SetPWM(IN1_CH, 255);
    SetPWM(IN2_CH, 255);
    SetPWM(IN3_CH, 255);
    SetPWM(IN4_CH, 255);
}

