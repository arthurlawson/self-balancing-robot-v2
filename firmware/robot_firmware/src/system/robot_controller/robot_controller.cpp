#include <config.h>
#include "robot_controller.h"

RobotController::RobotController(ImuService& imuSvc, LedService& ledSvc, PowerService& pwrSvc, DRV8833& motors, PidController& pid, 
                                 KalmanFilter& kf, AudioDriver& speaker)
    : _imuSvc(imuSvc), _ledSvc(ledSvc), _pwrSvc(pwrSvc), _motors(motors), _pid(pid), _kf(kf), _curState(RobotState::FALLEN),
      _speaker(speaker) {}

void RobotController::Begin() {
    _curState = RobotState::FALLEN;
    Reset();
}

void RobotController::Update() {
    // ===========================================================================
    // GET ORIENTATION
    // ===========================================================================
    float curAngle = _imuSvc.GetFilteredRoll();
    float safetyError = fabs(curAngle - DEFAULT_SETPOINT);

    // ===========================================================================
    // SAFETY CHECKS AND FALLEN CHECKS
    // ===========================================================================
    // Bootup alignment check
    if (!_ledSvc.IsBootComplete()) {
        _motors.StopBoth();
        return;
    }

    // Battery low check
    if (_curState == RobotState::BATTERY_LOW || _pwrSvc.IsBatteryLow()) {
        Reset();
        _curState = RobotState::BATTERY_LOW;
        _ledSvc.SetState(LedService::LED_BLINK_SLOW);
        _speaker.StopPlayback();
        return;
    }  

    // Fallen Check
    if (_curState != RobotState::FALLEN) {
        if (safetyError > STOP_ANGLE) {
            Reset();
            _curState = RobotState::FALLEN;
            _speaker.PlayRandomAudio();
            _ledSvc.SetState(LedService::LED_PULSING);
            return;
        }
    } else {
        if (safetyError <= ACTIVATION_ANGLE && _imuSvc.IsUpright()) {
            Reset();
            _speaker.StopPlayback();
            _ledSvc.SetState(LedService::LED_ON);
            _curState = RobotState::IDLE;
        } else {
            _motors.StopBoth();
            _ledSvc.SetState(LedService::LED_PULSING);
            return;
        }
    }    

    // ===========================================================================
    // INPUT AND VELOCITY TRACKING
    // ===========================================================================
    bool isDriving = (_curState == RobotState::FORWARD || _curState == RobotState::FORWARD_LEFT || _curState == RobotState::FORWARD_RIGHT ||
                      _curState == RobotState::BACKWARD || _curState == RobotState::BACKWARD_LEFT || _curState == RobotState::BACKWARD_RIGHT);

    int currentDriveDir = 0;
    float rawTarget = 0.0f;

    if (isDriving) {
        if (_curState == RobotState::FORWARD || _curState == RobotState::FORWARD_LEFT || _curState == RobotState::FORWARD_RIGHT) {
            rawTarget = MAX_DRIVE_LEAN_DEG;
            currentDriveDir = 1;
        } else {
            rawTarget = -MAX_DRIVE_LEAN_DEG;
            currentDriveDir = -1;
        }
    }

    bool isFlippingDirection = (currentDriveDir == -1 && _speedLeakAccumulator < -SPEED_TO_CONSIDER_BRAKE) || 
                               (currentDriveDir == 1 && _speedLeakAccumulator > SPEED_TO_CONSIDER_BRAKE);

    // ===========================================================================
    // TARGET LEAN AND SPEED MANAGEMENT
    // ===========================================================================
    float targetLeanAngle = DEFAULT_SETPOINT;

    if (isDriving) {
        _isDrivingInSameDir = ((rawTarget > 0.0f && _speedLeakAccumulator < -0.05f) ||
                               (rawTarget < 0.0f && _speedLeakAccumulator > 0.05f)) && !_isBrakingLock;
        
        if (_isDrivingInSameDir) {
            float speedLimitFactor = constrain(1.0f - (fabs(_speedLeakAccumulator) * 0.75f), 0.2f, 1.0f);
            targetLeanAngle = DEFAULT_SETPOINT + (rawTarget * speedLimitFactor);
        } else {
            _speedLeakAccumulator = 0.0f; 
            targetLeanAngle = DEFAULT_SETPOINT + rawTarget;
        }
    } else {
        _isBrakingLock = false;

        if (_currentTargetLean > DEFAULT_SETPOINT + 0.5f)      _currentBrakingLean = -1.5f; 
        else if (_currentTargetLean < DEFAULT_SETPOINT - 0.5f) _currentBrakingLean = 1.5f;
        else                                                    _currentBrakingLean *= 0.85f;
    }

    float dynamicTarget = targetLeanAngle + _currentBrakingLean;

    // Determine target interpolation speed
    float activeRampRate = MOVEMENT_RAMP_RATE;
    if (dynamicTarget == DEFAULT_SETPOINT || fabs(dynamicTarget - DEFAULT_SETPOINT) < fabs(_currentTargetLean - DEFAULT_SETPOINT)) {
        activeRampRate = BRAKING_RAMP_RATE; 
    }
    
    // If its flipping direction, dont interpolate slowly, else interpolate slowly
    if (isFlippingDirection) {
        _currentTargetLean = dynamicTarget;
        _isBrakingLock = true;
    } else {
        _currentTargetLean = ((1.0f - activeRampRate) * _currentTargetLean) + (activeRampRate * dynamicTarget);
    }

    // ===========================================================================
    // GAIN SCHEDULING
    // ===========================================================================
    float currentKp = KP, currentKd = KD, currentMinPwm = MIN_PWM;

    if (isFlippingDirection && safetyError > 0.8f) {
        currentKp *= 5.0f; 
        currentKd *= 4.0f; 
        currentMinPwm += 40.0f; 
    }

    _pid.SetKp(constrain(currentKp, 0.0f, 220.0f));
    _pid.SetKd(constrain(currentKd, 0.0f, 3.5f));
    currentMinPwm = constrain(currentMinPwm, 0.0f, 160.0f);

    // ===========================================================================
    // YAW CONFIGURATION (FOR TURNING)
    // ===========================================================================
    if (_curState == RobotState::LEFT) _yawTurnOffset = TURN_SPEED_OFFSET;
    else if (_curState == RobotState::RIGHT) _yawTurnOffset = -TURN_SPEED_OFFSET;
    else if (_curState == RobotState::FORWARD_LEFT || _curState == RobotState::BACKWARD_LEFT) _yawTurnOffset = TURN_SPEED_OFFSET * 0.35f;
    else if (_curState == RobotState::FORWARD_RIGHT || _curState == RobotState::BACKWARD_RIGHT) _yawTurnOffset = -TURN_SPEED_OFFSET * 0.35f; 
    else _yawTurnOffset = 0.0f;    

    bool isPureSpin = ((_curState == RobotState::LEFT || _curState == RobotState::RIGHT) && targetLeanAngle == DEFAULT_SETPOINT);
    static float smoothedYawOffset = 0.0f;
    smoothedYawOffset = isPureSpin ? _yawTurnOffset : (0.90f * smoothedYawOffset) + (0.10f * _yawTurnOffset); 

    // ===========================================================================
    // PID COMPUTATION AND DEADBAND
    // ===========================================================================
    float virtualAngle = curAngle - _currentTargetLean;
    float balancingOutput = _pid.Compute(virtualAngle, 255.0f);

    // Leak speed when not being controlled
    if ((isDriving && _isDrivingInSameDir) || _currentBrakingLean != 0.0f) {
        _speedLeakAccumulator = (0.98f * _speedLeakAccumulator) + (0.02f * (balancingOutput / 255.0f));
    } else {
        _speedLeakAccumulator *= 0.90f; 
        if (fabs(_speedLeakAccumulator) < 0.01f) _speedLeakAccumulator = 0.0f;
    }

    // Deadband injection
    if (fabs(virtualAngle) > 0.05) {
        if (balancingOutput > 0.0f) balancingOutput += currentMinPwm;
        else if (balancingOutput < 0.0f) balancingOutput -= currentMinPwm;
    } else {
        balancingOutput = 0.0f;
    }
    balancingOutput = constrain(balancingOutput, -255.0f, 255.0f);

    // ===========================================================================
    // MOTOR OUTPUT WITH LEFT AND RIGHT MIXING
    // ===========================================================================
    float turnIntensity = fabs(smoothedYawOffset) / 255.0f;
    
    float dynamicCap = 1.0f - (turnIntensity * 1.1f);
    if (dynamicCap < 0.15f) dynamicCap = 0.15f;
    
    float cappedBalance = balancingOutput * dynamicCap;

    float leftMotorPower  = cappedBalance;
    float rightMotorPower = cappedBalance;

    if (smoothedYawOffset > 0.0f) {
        rightMotorPower = cappedBalance - (smoothedYawOffset * 1.6f);
    } 
    else if (smoothedYawOffset < 0.0f) {
        leftMotorPower = cappedBalance + (smoothedYawOffset * 1.6f); 
    }

    leftMotorPower *= LEFT_TRIM;
    rightMotorPower *= RIGHT_TRIM;

    leftMotorPower = constrain(leftMotorPower, -255.0f, 255.0f);
    rightMotorPower = constrain(rightMotorPower, -255.0f, 255.0f);

    int16_t leftDrive = static_cast<int16_t>(leftMotorPower);
    int16_t rightDrive = static_cast<int16_t>(rightMotorPower);

    _motors.Drive(leftDrive, rightDrive);
}

void RobotController::SetState(RobotState state) {
    if (_curState == RobotState::FALLEN || _curState == RobotState::BATTERY_LOW) return;

    _curState = state;
}

void RobotController::Reset() {
    _currentTargetLean = DEFAULT_SETPOINT;
    _currentBrakingLean = 0.0f;
    _speedLeakAccumulator = 0.0f;
    _isBrakingLock = false;
    _isDrivingInSameDir = false;

    _imuSvc.Reset();
    _motors.StopBoth();
    _pid.Reset();
    _kf.Reset();
}