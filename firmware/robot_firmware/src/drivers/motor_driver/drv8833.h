/**
 * @file drv8833.h
 * @brief DRV8833 motor driver class for controlling two DC motors.
 * 
 * @author Arthur Lawson
 * 
 */

#pragma once

#include <Arduino.h> 

class DRV8833 {
    public: 
        DRV8833(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);
        void Drive(int16_t left, int16_t right);
        void StopBoth();
    private: 
        uint8_t _in1, _in2, _in3, _in4;
};
