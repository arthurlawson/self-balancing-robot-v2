/**
 * @file math_utils.h
 * @brief Shared mathematical utility functions.
 */

#pragma once

#include <cstdint>

static inline int16_t Clamp(int16_t value, int16_t max) {
    if (value < -max) return -max;
    if (value > max)  return max;
    return value;
}

static inline float Sqr(float x) { return x * x; }