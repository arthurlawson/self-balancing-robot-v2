#include "imu_service.h"

ImuService::ImuService(KalmanFilter& filter) 
    : _filter(filter) {}

bool ImuService::Begin(int sda, int scl) {
    if (!Wire.begin(sda, scl)) return false;
    Wire.setClock(400000); // 400kHz for quick I2C updates

    if (!_mpu.begin()) {
        return false;
    }

    _mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    _mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    _mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

    return true;
}

void ImuService::Update() {
    if (_mpu.getEvent(&_accel, &_gyro, &_temp)) {
        float gyroRollRateDeg = _gyro.gyro.x * RAD_TO_DEG;

        float ax = _accel.acceleration.x;
        float ay = _accel.acceleration.y;
        float az = _accel.acceleration.z;
        float accelRollDeg = atan2(ay, sqrt((ax * ax) + (az * az))) * RAD_TO_DEG;

        _filter.Predict(gyroRollRateDeg);
        _filter.MeasurementTask(accelRollDeg);
    }
}