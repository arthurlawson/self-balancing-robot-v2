#include <config.h>
#include "robot_controller.h"

RobotController::RobotController(ImuService& imuSvc, LedService& ledSvc, PowerService& pwrSvc, DRV8833& motors, PidController& pid, KalmanFilter& kf)
    : _imuSvc(imuSvc), _ledSvc(ledSvc), _pwrSvc(pwrSvc), _motors(motors), _pid(pid), _kf(kf), _curState(RobotState::FALLEN),
      _driveSpeedOffset(0.0f), _yawTurnOffset(0.0f) {}

void RobotController::Begin() {
    _motors.StopBoth();
    _curState = RobotState::FALLEN;
}

void RobotController::SetState(RobotState state) {
    // Ignore state changes if battery is low / the robot has fallen
    if (_curState == RobotState::BATTERY_LOW || _curState == RobotState::FALLEN) return;
    _curState = state;
}

void RobotController::Update() {
    // Battery Check
    if (_curState == RobotState::BATTERY_LOW || _pwrSvc.IsBatteryLow()) {
        _curState = RobotState::BATTERY_LOW;
        _motors.StopBoth();
        _pid.Reset();
        _ledSvc.SetState(LedService::LED_BLINK_SLOW);
        return;
    }  

    // Bootup alignment
    if (!_ledSvc.IsBootComplete()) {
        _motors.StopBoth();
        return;
    }

    // Orientation calculations
    float curAngle = _imuSvc.GetFilteredRoll();
    float error = fabs(curAngle - DEFAULT_SETPOINT);

    // Fallen Check
    if (_curState != RobotState::FALLEN) {
        if (error > STOP_ANGLE) {
            _curState = RobotState::FALLEN;
            _imuSvc.Reset();
            _ledSvc.SetState(LedService::LED_PULSING);
            _motors.StopBoth();
            _pid.Reset();
            _kf.Reset();
            return;
        }
    } else {
        if (error <= ACTIVATION_ANGLE) {
            _pid.Reset();
            _kf.Reset();
            _curState = RobotState::IDLE;
        } else {
            _motors.StopBoth();
            _ledSvc.SetState(LedService::LED_PULSING);
            return;
        }
    }

    // Motion coordination for each state
    static constexpr float DRIVE_BASE_OFFSET = 45.0f; // Constant initial torque
    
    switch (_curState) {
        case RobotState::FORWARD:
            _driveSpeedOffset = DRIVE_BASE_OFFSET;
            _yawTurnOffset = 0.0f;
            break;
        case RobotState::BACKWARD:
            _driveSpeedOffset = -DRIVE_BASE_OFFSET;
            _yawTurnOffset = 0.0f;
            break;
        case RobotState::LEFT:
            _driveSpeedOffset = 0.0f;
            _yawTurnOffset = -TURN_SPEED_OFFSET; 
            break;
        case RobotState::RIGHT:
            _driveSpeedOffset = 0.0f;
            _yawTurnOffset = TURN_SPEED_OFFSET;  
            break;
        case RobotState::FORWARD_LEFT:
            _driveSpeedOffset = DRIVE_BASE_OFFSET;
            _yawTurnOffset = -TURN_SPEED_OFFSET * 0.6f; 
            break;
        case RobotState::FORWARD_RIGHT:
            _driveSpeedOffset = DRIVE_BASE_OFFSET;
            _yawTurnOffset = TURN_SPEED_OFFSET * 0.6f;  
            break;
        case RobotState::BACKWARD_LEFT:
            _driveSpeedOffset = -DRIVE_BASE_OFFSET;
            _yawTurnOffset = -TURN_SPEED_OFFSET * 0.6f; 
            break;
        case RobotState::BACKWARD_RIGHT:
            _driveSpeedOffset = -DRIVE_BASE_OFFSET;
            _yawTurnOffset = TURN_SPEED_OFFSET * 0.6f;  
            break;
        case RobotState::IDLE:
        default:
            _driveSpeedOffset = 0.0f; 
            _yawTurnOffset = 0.0f;
            break;
    } 

    // Set normal active lighting
    if (_curState != RobotState::FALLEN && _curState != RobotState::BATTERY_LOW) {
        _ledSvc.SetState(LedService::LED_ON);
    }

    // PID routine
    _pid.SetSetpoint(DEFAULT_SETPOINT);
    float balancingOutput = _pid.Compute(curAngle, 255.0f);

    // Motor output
    int16_t leftDrive = static_cast<int16_t>((balancingOutput + _driveSpeedOffset + _yawTurnOffset));
    int16_t rightDrive = static_cast<int16_t>((balancingOutput + _driveSpeedOffset - _yawTurnOffset));
    _motors.Drive(leftDrive, rightDrive);
}