/**
 * @file time_utils.h
 * @brief Utility functions for system timing.
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <cstdint>
#include "esp_timer.h"

static inline uint32_t GetMillis() {
    // Reads the 64-bit hardware counter in microseconds and scales to milliseconds
    return static_cast<uint32_t>(esp_timer_get_time() / 1000);
}