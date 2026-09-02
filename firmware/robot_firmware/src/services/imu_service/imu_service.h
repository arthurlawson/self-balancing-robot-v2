/**
 * @file imu_service.h
 * @brief IMU service class to manage MPU6050 data and Kalman Filtering.
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "utils/kalman_filter/kalman_filter.h"

class ImuService {
    public:
        ImuService(KalmanFilter& filter);
        bool Begin(int sda, int scl);
        void Update();
        void Reset() { _filter.Reset(); }
        float GetFilteredRoll() const { return -_filter.GetRoll(); } // negative due to orientation of the IMU
    private:
        Adafruit_MPU6050 _mpu;
        KalmanFilter& _filter;
        sensors_event_t _accel, _gryo, _temp;
};