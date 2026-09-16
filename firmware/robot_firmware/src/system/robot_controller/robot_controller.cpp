#include <config.h>
#include "robot_controller.h"

RobotController::RobotController(ImuService& imuSvc, LedService& ledSvc, PowerService& pwrSvc, DRV8833& motors, PidController& pid, 
                                 KalmanFilter& kf, AudioDriver& speaker)
    : _imuSvc(imuSvc), _ledSvc(ledSvc), _pwrSvc(pwrSvc), _motors(motors), _pid(pid), _kf(kf), _curState(RobotState::FALLEN),
      _speaker(speaker) {}

void RobotController::Begin() {
    _motors.StopBoth();
    _curState = RobotState::FALLEN;
    _currentTargetLean = DEFAULT_SETPOINT;
    _currentBrakingLean = 0.0f;
    _yawTurnOffset = 0.0f;
    _isDrivingInSameDir = false;
    _speedLeakAccumulator = 0.0f;
    _lastDriveDir = 0;
    _isBrakingLock = false;
}

void RobotController::Update() {
    // Battery Check
    if (_curState == RobotState::BATTERY_LOW || _pwrSvc.IsBatteryLow()) {
        _curState = RobotState::BATTERY_LOW;
        _motors.StopBoth();
        _pid.Reset();
        _ledSvc.SetState(LedService::LED_BLINK_SLOW);
        _speaker.StopPlayback();
        return;
    }  

    // Bootup alignment
    if (!_ledSvc.IsBootComplete()) {
        _motors.StopBoth();
        return;
    }

    // Lean Axis
    float targetLeanAngle = DEFAULT_SETPOINT;
    int currentDriveDir = 0;

    bool isDriving = (_curState == RobotState::FORWARD || _curState == RobotState::FORWARD_LEFT || _curState == RobotState::FORWARD_RIGHT ||
                      _curState == RobotState::BACKWARD || _curState == RobotState::BACKWARD_LEFT || _curState == RobotState::BACKWARD_RIGHT);

    float rawTarget = 0.0f;

    if (isDriving) {
        if (_curState == RobotState::FORWARD || _curState == RobotState::FORWARD_LEFT || _curState == RobotState::FORWARD_RIGHT) {
            rawTarget = MAX_DRIVE_LEAN_DEG;
            currentDriveDir = 1;
        } else {
            rawTarget = -MAX_DRIVE_LEAN_DEG;
            currentDriveDir = -1;
        }

        bool isExplicitCounterSteer = (currentDriveDir == 1 && _lastDriveDir == -1) ||
                                      (currentDriveDir == -1 && _lastDriveDir == 1);

         _isDrivingInSameDir = ((rawTarget > 0.0f && _currentTargetLean > DEFAULT_SETPOINT + 0.1f) ||
                               (rawTarget < 0.0f && _currentTargetLean < DEFAULT_SETPOINT - 0.1f));
        
        if (_isDrivingInSameDir) {
            float speedLimitFactor = 1.0f - (fabs(_speedLeakAccumulator) * 0.75f);
            if (speedLimitFactor < 0.20f) speedLimitFactor = 0.20f;

            targetLeanAngle = DEFAULT_SETPOINT + (rawTarget * speedLimitFactor);
        } else {
            if (isExplicitCounterSteer) {
                _speedLeakAccumulator = 0.0f; 
            }
            targetLeanAngle = DEFAULT_SETPOINT + rawTarget;
        }
    }
    else {
        targetLeanAngle = DEFAULT_SETPOINT;
        currentDriveDir = 0;

        if (_currentTargetLean > DEFAULT_SETPOINT + 0.5f) {
            _currentBrakingLean = -1.5f; 
        }
        else if (_currentTargetLean < DEFAULT_SETPOINT - 0.5f) {
            _currentBrakingLean = 1.5;
        }
        else {
            _currentBrakingLean *= 0.85f; 
        }
    }

    float dynamicTarget = targetLeanAngle + _currentBrakingLean;

    // Determine target interpolation speed
    float activeRampRate = MOVEMENT_RAMP_RATE;
    if (dynamicTarget == DEFAULT_SETPOINT || 
        fabs(dynamicTarget - DEFAULT_SETPOINT) < fabs(_currentTargetLean - DEFAULT_SETPOINT)) {
        activeRampRate = BRAKING_RAMP_RATE; 
    }

    bool isExplicitCounterSteer = (currentDriveDir == 1 && _lastDriveDir == -1) ||
                                  (currentDriveDir == -1 && _lastDriveDir == 1);
    
    if (isExplicitCounterSteer) {
        _currentTargetLean = dynamicTarget;
        _isBrakingLock = true;
    } else {
        _currentTargetLean = ((1.0f - activeRampRate) * _currentTargetLean) + (activeRampRate * dynamicTarget);
    }

    // Keep memory of the state (Ignoring Idle periods)
    if (isDriving) {
        _lastDriveDir = currentDriveDir;
    }

    // Turn Axis
    if (_curState == RobotState::LEFT) {
        _yawTurnOffset = TURN_SPEED_OFFSET;
    } 
    else if (_curState == RobotState::RIGHT) {
        _yawTurnOffset = -TURN_SPEED_OFFSET;
    } 
    else if (_curState == RobotState::FORWARD_LEFT || _curState == RobotState::BACKWARD_LEFT) {
        _yawTurnOffset = TURN_SPEED_OFFSET * 0.35f;
    } 
    else if (_curState == RobotState::FORWARD_RIGHT || _curState == RobotState::BACKWARD_RIGHT) {
        _yawTurnOffset = -TURN_SPEED_OFFSET * 0.35f; 
    } 
    else {
        _yawTurnOffset = 0.0f; // No active turning component
    }

    // Orientation calculations
    float curAngle = _imuSvc.GetFilteredRoll();
    float safetyError = fabs(curAngle - DEFAULT_SETPOINT);

    // Fallen Check
    if (_curState != RobotState::FALLEN) {
        if (safetyError > STOP_ANGLE) {
            _curState = RobotState::FALLEN;
            _currentTargetLean = DEFAULT_SETPOINT;
            _currentBrakingLean = 0.0f;
            _speedLeakAccumulator = 0.0f;
            _lastDriveDir = 0; // IDLE
            _isBrakingLock = false;
            _isDrivingInSameDir = false;

            _imuSvc.Reset();
            _ledSvc.SetState(LedService::LED_PULSING);
            _motors.StopBoth();
            _pid.Reset();
            _kf.Reset();
            _speaker.PlayRandomAudio();

            return;
        }
    } else {
        if (safetyError <= ACTIVATION_ANGLE && _imuSvc.IsUpright()) {
            _pid.Reset();
            _kf.Reset();
            _speaker.StopPlayback();
            _curState = RobotState::IDLE;
        } else {
            _motors.StopBoth();
            _ledSvc.SetState(LedService::LED_PULSING);
            return;
        }
    }

    // Set normal active lighting
    _ledSvc.SetState(LedService::LED_ON);

    // Gain Schedule
    bool isPureSpin = ((_curState == RobotState::LEFT || _curState == RobotState::RIGHT) && targetLeanAngle == DEFAULT_SETPOINT);

    float currentKp = KP;
    float currentKd = KD;
    float currentMinPwm = MIN_PWM;

    static float smoothedYawOffset = 0.0f;
    if (isPureSpin) {
        smoothedYawOffset = _yawTurnOffset;
    } else {
        smoothedYawOffset = (0.90f * smoothedYawOffset) + (0.10f * _yawTurnOffset); 
    }

    // Coefficients updated to avoid oscillations
    if (isExplicitCounterSteer && safetyError > 1.0f) {
        currentKp *= 5.0f; 
        currentKd *= 4.0f; 
        currentMinPwm += 40.0f; 
    }

    currentKp = constrain(currentKp, 0.0f, 220.0f);
    currentKd = constrain(currentKd, 0.0f, 3.5f);
    currentMinPwm = constrain(currentMinPwm, 0.0f, 160.0f);

    // PID computations
    _pid.SetKp(currentKp);
    _pid.SetKd(currentKd);

    float virtualAngle = curAngle - _currentTargetLean;
    float balancingOutput = _pid.Compute(virtualAngle, 255.0f);

    if (isDriving) {
        bool samePhysicalDir = (currentDriveDir == 1 && _currentTargetLean > DEFAULT_SETPOINT + 0.1f) || 
                               (currentDriveDir== -1 && _currentTargetLean < DEFAULT_SETPOINT - 0.1f);

        _isDrivingInSameDir = (samePhysicalDir && !_isBrakingLock);

        if (_isDrivingInSameDir) {
            _speedLeakAccumulator = (0.98f * _speedLeakAccumulator) + (0.02f * (balancingOutput / 255.0f));
            
            float speedLimitFactor = 1.0f - (fabs(_speedLeakAccumulator) * 0.75f);
            if (speedLimitFactor < 0.20f) speedLimitFactor = 0.20f;

            float limitedTarget = DEFAULT_SETPOINT + (rawTarget * speedLimitFactor) + _currentBrakingLean;
            virtualAngle = curAngle - limitedTarget;
            balancingOutput = _pid.Compute(virtualAngle, 255.0f);
        } else {
            if (isExplicitCounterSteer) {
                _speedLeakAccumulator = 0.0f; 
            }
        }
    } else {
        _isBrakingLock = false;
        _speedLeakAccumulator *= 0.96f;
    }

    // Deadband injection
    if (fabs(virtualAngle) > 0.05) {
        if (balancingOutput > 0.0f) balancingOutput += currentMinPwm;
        else if (balancingOutput < 0.0f) balancingOutput -= currentMinPwm;
    } else {
        balancingOutput = 0.0f;
    }
    
    balancingOutput = constrain(balancingOutput, -255.0f, 255.0f);

    // Reduces balancing slightly during high speed turns, giving headroom
    float balanceScale = 1.0f - (fabs(smoothedYawOffset) / 255.0f) * 0.7f;
    float scaledBalance = balancingOutput * balanceScale;

    // Motor output
    int16_t leftDrive = 0;
    int16_t rightDrive = 0;

    leftDrive  = static_cast<int16_t>((scaledBalance * LEFT_TRIM) + LEFT_TRIM);
    rightDrive = static_cast<int16_t>((scaledBalance * RIGHT_TRIM) - RIGHT_TRIM);

    _motors.Drive(leftDrive, rightDrive);
}

void RobotController::SetState(RobotState state) {
    if (_curState == RobotState::FALLEN || _curState == RobotState::BATTERY_LOW) return;

    _curState = state;
}