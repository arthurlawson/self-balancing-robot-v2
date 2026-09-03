/**
 * @file pid_controller.h
 * @brief PID Control logic with a low pass filter
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>

class PidController {
    public:
        PidController(float setpoint, float kp, float ki, float kd, float dtSec);
        float Compute(float pv, float maxOutput);
        void SetSetpoint(float setpoint);
        void SetKp(float kp);
        void SetKd(float kd);
        float GetDerivative() const;
        void LeakIntegral(float percentage);
        void Reset();
    private:
        float _kp, _kd, _ki, _dtSec;
        float _setpoint;
        float _integral;
        float _previousError;
        float _previousDerivative;

        // Pre allocated variables
        float _error;
        float _derivative;
        float _out;
};