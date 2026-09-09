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
        PidController();
        float Compute(float pv, float maxOutput);
        void SetKp(float kp);
        void SetKd(float kd);
        float GetDerivative() const;
        void LeakIntegral(float percentage);
        void Reset();

    private:
        float _kp, _kd, _ki;
        float _integral;
        float _previousError;
        float _previousDerivative;

        // Pre allocated variables
        float _error;
        float _derivative;
        float _out;
};