/**
 * @file protocol.h
 * @brief Communication protocol for the ESP32-S3 Self-Balancing Robot.
 */

#pragma once

typedef struct {
    char command; // 'F' = Forward, 'B' = Backward, 'L' = Left, 'R' = Right
} ControlPacket;
