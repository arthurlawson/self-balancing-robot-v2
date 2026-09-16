/**
 * @file main.cpp
 * @brief Entry point for the Self Balancing Robot Remote
 * 
 * @author Arthur Lawson
 */

#include <Arduino.h>
#include <config.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "protocol.h"

ControlPacket msg;
esp_now_peer_info_t peerInfo;
unsigned long lastAckTime = 0;

void OnDataSent(const uint8_t* mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        lastAckTime = millis();
    }
}

void setup() {
    Serial.begin(115200);
    // LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Buttons
    pinMode(FWD_PIN, INPUT_PULLUP);
    pinMode(BWD_PIN, INPUT_PULLUP);
    pinMode(LFT_PIN, INPUT_PULLUP);
    pinMode(RGT_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);

    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) return;

    esp_now_register_send_cb(OnDataSent);

    // Register peer
    memcpy(peerInfo.peer_addr, BROADCAST_ADDRESS, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
        return;
    }

    Serial.println("ESP-NOW initialized and peer successfully added!");
}

void loop() {
  bool fwd = (digitalRead(FWD_PIN) == LOW);
  bool bwd = (digitalRead(BWD_PIN) == LOW);
  bool lft = (digitalRead(LFT_PIN) == LOW);
  bool rgt = (digitalRead(RGT_PIN) == LOW);

  if (fwd && bwd) { fwd = false; bwd = false; }
  if (lft && rgt) { lft = false; rgt = false; }

  if (fwd && lft)        msg.command = '1';
  else if (fwd && rgt)   msg.command = '2';
  else if (bwd && lft)   msg.command = '3';
  else if (bwd && rgt)   msg.command = '4';
  else if (fwd)          msg.command = 'F';
  else if (bwd)          msg.command = 'B';
  else if (lft)          msg.command = 'L';
  else if (rgt)          msg.command = 'R';
  else                   msg.command = 'I';

  if (millis() - lastAckTime < 200)
  {
    digitalWrite(LED_PIN, HIGH);
  } else {
        digitalWrite(LED_PIN, LOW);
    }

    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > 50) { // send every 50ms - Half of watchdog timout in robot firmware
        lastSendTime = millis();
        esp_now_send(BROADCAST_ADDRESS, (uint8_t*) &msg, sizeof(msg));

        if (msg.command != 'I') {
            Serial.printf("Transmitting: %c\n", msg.command);
        }
    }
}