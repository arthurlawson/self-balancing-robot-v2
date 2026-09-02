    #include <config.h>
    #include "kalman_filter.h"
    #include <math.h>

    static inline float Sqr(float x) { return x * x; }

    KalmanFilter::KalmanFilter(float dtSec) : _dtSec(dtSec)
    {
        Reset();
    }

    void KalmanFilter::Predict(const float gyroRollRateDeg)
    {
        // Subtract the estimated gyro bias to get an unbiased rotation rate
        float unbiasedGyroRate = gyroRollRateDeg - _rollBias;
        _rollAngle += unbiasedGyroRate * _dtSec;

        // Error covariance matrix prediction step expanded (P = A*P*A' + Q)
        _pRoll00 += (_dtSec * (_dtSec * _pRoll11 - _pRoll01 - _pRoll10)) + Q_INIT;
        _pRoll01 -= _dtSec * _pRoll11;
        _pRoll10 -= _dtSec * _pRoll11;
        _pRoll11 += R_BIAS;
    }

    void KalmanFilter::MeasurementTask(const float accelRollDeg) {
        if (!_isInitialized) {
            if (fabs(accelRollDeg) <= ACTIVATION_ANGLE) {
                _rollAngle = accelRollDeg;
                _rollBias = 0.0f;
                _isInitialized = true;
            } else {
                _rollAngle = 0.0f;
                _rollBias = 0.0f;
                return;
            }
        }

        // Calculate the innovation (the error between the absolute accelerometer angle and the prediction)
        float innovation = accelRollDeg - _rollAngle;

        // Total measurement uncertainty (the innovation variance)
        float S = _pRoll00 + R_INIT;

        // Calculate optimal Kalman gains for both states
        float k0 = _pRoll00 / S;    // gain for scaling angle corrections
        float k1 = _pRoll10 / S;    // gain for scaling gyro bias updates

        // Update the state variables
        _rollAngle += k0 * innovation;
        _rollBias += k1 * innovation;

        // Update the covariance P matrix
        float p00Temp = _pRoll00;
        float p01Temp = _pRoll01;
        _pRoll00 -= k0 * p00Temp;
        _pRoll01 -= k0 * p01Temp;
        _pRoll10 -= k1 * p00Temp;
        _pRoll11 -= k1 * p01Temp;

        // Force covariance symmetry (rounding errors can cause divergence)
        _pRoll10 = _pRoll01;
    }

    float KalmanFilter::GetRoll() const {
        return _rollAngle;
    }

    void KalmanFilter::Reset() {
        _isInitialized = false;
        _rollAngle = 0.0f;
        _rollBias = 0.0f;

        _pRoll00 = SIGMA_INIT;
        _pRoll01 = 0.0f;
        _pRoll10 = 0.0f;
        _pRoll11 = SIGMA_INIT;
    }