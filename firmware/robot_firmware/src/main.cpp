/**
 * @file main.cpp
 * @brief Entry point for the Self Balancing Robot
 * 
 * @author Arthur Lawson
 */

#include <Arduino.h>
#include <config.h>
#include "utils/kalman_filter/kalman_filter.h"
#include "utils/pid_controller/pid_controller.h"
#include "drivers/motor_driver/drv8833.h"
#include "drivers/audio_driver/audio_driver.h"
#include "services/imu_service/imu_service.h"
#include "services/led_service/led_service.h"
#include "services/power_service/power_service.h"
#include "system/robot_controller/robot_controller.h"

// UTILS
KalmanFilter kf(DELTA_T_SEC);
PidController pid(DEFAULT_SETPOINT, KP, KI, KD, DELTA_T_SEC);

// DRIVERS
DRV8833 motors(IN1_PIN, IN2_PIN, IN3_PIN, IN4_PIN);
AudioDriver speakers(SPEAKER_PIN);

// SERVICES
ImuService imuSvc(kf);
LedService ledSvc(LED_PIN);
PowerService pwrSvc(BATT_PIN, ledSvc, BATT_THRESHOLD);

// ROBOT CONTROL SYSTEMS
RobotController robot(imuSvc, ledSvc, pwrSvc, motors, pid, kf);

// HARDWARE TIMER
volatile bool timerFlag = false;
hw_timer_t *timer = NULL;

void ARDUINO_ISR_ATTR onTimer() {
  timerFlag = true;
}

void setup() {
  Serial.begin(115200);

  if (!imuSvc.Begin(SDA_PIN, SCL_PIN)) {
    while (1) {
      Serial.println("IMU Initialization Failed.");
      delay(1000);
    }
  }

  ledSvc.Begin();
  pwrSvc.Begin();
  speakers.Begin();
  robot.Begin();

  timer = timerBegin(0, TIMER_PRESCALER, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, TIMER_ALARM_US, true);
  timerAlarmEnable(timer);

  Serial.println("System Core Ready!");
}

void loop() {
  speakers.Update();

  if (timerFlag) {
    timerFlag = false;

    ledSvc.Update();
    imuSvc.Update();
    robot.Update();
  }
}