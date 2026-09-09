#include <Arduino.h>
#include <config.h>
#include <math.h>
#include "pid_controller.h"

PidController::PidController() {

    _kp = KP;
    _kd = KD;
    _ki = KI;

    Reset();

}

float PidController::Compute(float pv, float maxOutput) {
    _error = DEFAULT_SETPOINT - pv;

    // Derivative with low pass filter
    float rawDerivative = (_error - _previousError) / DELTA_T_SEC;
    _derivative = (0.80f * _previousDerivative) + (0.20f * rawDerivative);

    _previousError = _error;
    _previousDerivative = _derivative;

    // Integral windup prevention
    _integral += _error * DELTA_T_SEC;
    float maxIntegralBound = 50.0f / (_ki > 0.0f ? _ki : 1.0f);
    _integral = constrain(_integral, -maxIntegralBound, maxIntegralBound);

    // Generate output
    _out = (_kp * _error) + (_ki * _integral) + (_kd * _derivative);
    return constrain(_out, -maxOutput, maxOutput);
}
    
void PidController::SetKp(float kp) {
    _kp = kp;
}

void PidController::SetKd(float kd) {
    _kd = kd;
}

float PidController::GetDerivative() const { 
    return _derivative; 
}

void PidController::LeakIntegral(float percentage) { 
    _integral *= percentage; 
}

void PidController::Reset() {
    _integral = 0.0f;
    _derivative = 0.0f;
    _error = 0.0f;
    _previousError = 0.0f;
    _previousDerivative = 0.0f;
    _out = 0.0f;
}