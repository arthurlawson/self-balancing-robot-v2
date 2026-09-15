#include "remote_service.h"

// Init of static members
RobotController* RemoteService::_robot = nullptr;
unsigned long RemoteService::_lastPacketTime = 0;

void RemoteService::Begin(RobotController& robot) {
    _robot = &robot;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); 

    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error: ESP-NOW initialisation failed!");
        return;
    }

    // callback function that runs when the data arrives
    esp_now_register_recv_cb(OnDataReceived);
    _lastPacketTime = millis();
}

void RemoteService::Update() {
    // Safety watchdog protection forces robot to idle state
    if (millis() - _lastPacketTime > WATCHDOG_TIMOUT_MS) {
        if (_robot != nullptr) {
            _robot->SetState(RobotController::RobotState::IDLE);
        }
    }
}

void RemoteService::OnDataReceived(const uint8_t* mac, const uint8_t* data, int len) {
    ControlPacket packet;
    if (len != sizeof(packet)) return;

    memcpy(&packet, data, sizeof(ControlPacket));
    _lastPacketTime = millis(); // refresh watchdog timer

    if (_robot == nullptr) return;

    switch (packet.command) {
        case 'F': _robot->SetState(RobotController::RobotState::FORWARD); break;
        case 'B': _robot->SetState(RobotController::RobotState::BACKWARD); break;
        case 'L': _robot->SetState(RobotController::RobotState::LEFT); break;
        case 'R': _robot->SetState(RobotController::RobotState::RIGHT); break;

        case '1': _robot->SetState(RobotController::RobotState::FORWARD_LEFT); break;
        case '2': _robot->SetState(RobotController::RobotState::FORWARD_RIGHT); break;
        case '3': _robot->SetState(RobotController::RobotState::BACKWARD_LEFT); break;
        case '4': _robot->SetState(RobotController::RobotState::BACKWARD_RIGHT); break;

        case 'I': _robot->SetState(RobotController::RobotState::IDLE); break;
        default: _robot->SetState(RobotController::RobotState::IDLE); break;
    }
}
