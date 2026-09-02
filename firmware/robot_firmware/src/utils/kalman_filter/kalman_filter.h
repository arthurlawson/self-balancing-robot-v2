/** 
 * @file kalman_filter.h
 * @brief 2D Linear Kalman Filter tracking absolute tilt angle and gyroscope bias.
 * Sensor Fusion of the Accelerometer and Gyroscope data from the MPU6050 sensor to provide a stable output.
 * 
 * @author https://github.com/temperancee (Original logic)
 * @author Arthur Lawson (Refined for the MPU6050 sensor and adapted for this project)
 */

#pragma once

#include <Arduino.h>

class KalmanFilter {
    public:
        KalmanFilter(float dtSec);
        void Predict(const float gyroRollRateDeg);
        void MeasurementTask(const float accelRollDeg);
        float GetRoll() const;
        void Reset();
    private:
        float _dtSec;
        bool _isInitialized;

        // Tuning Parameters matching the MPU6050 noise profiles
        float SIGMA_INIT = 0.1f;        // Initial uncertainty guess
        float Q_INIT = 0.000000125f;    // Process noise (Gyroscope trust)
        float R_INIT = 0.0966f;         // Measurement noise (Accel trust)
        float R_BIAS = 0.0000001f;      // Gyro bias noise

        // State Variables (estimated output angle and gyro bias)
        float _rollAngle;
        float _rollBias;

        // 2x2 Covariance P Matrix to track state estimation uncertainty
        float _pRoll00;     // Angle variance
        float _pRoll01;     // Covariance between the estimated angle and bias
        float _pRoll10;     // Covariance between the estimated angle and bias
        float _pRoll11;     // Bias Variance
};
