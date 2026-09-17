/**
 * @file robot_controller.h
 * @brief The balancing brain of the robot with a state machine for movement
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>
#include "services/imu_service/imu_service.h"
#include "services/led_service/led_service.h"
#include "services/power_service/power_service.h"
#include "drivers/motor_driver/drv8833.h"
#include "drivers/audio_driver/audio_driver.h"
#include "utils/pid_controller/pid_controller.h"

class RobotController {
    public:
        enum class RobotState
        {
            IDLE,
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            FORWARD_LEFT,
            FORWARD_RIGHT,
            BACKWARD_LEFT,
            BACKWARD_RIGHT,
            FALLEN,
            BATTERY_LOW
        };

        RobotController(ImuService& imuSvc, LedService& ledSvc, PowerService& pwrSvc, DRV8833& motors, PidController& pid, 
                        KalmanFilter& kf, AudioDriver& speaker);             
        void Begin();
        void Update();
        void SetState(RobotState state);

    private:
        void Reset();

        ImuService& _imuSvc;
        LedService& _ledSvc;
        PowerService& _pwrSvc;
        DRV8833& _motors;
        PidController& _pid;
        KalmanFilter& _kf;
        AudioDriver& _speaker;

        RobotState _curState;
        float _yawTurnOffset;
        float _currentTargetLean;
        float _currentBrakingLean;
        bool _isDrivingInSameDir;
        float _speedLeakAccumulator;
        bool _isBrakingLock;
};
