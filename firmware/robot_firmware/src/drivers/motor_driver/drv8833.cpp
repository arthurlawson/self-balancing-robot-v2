#include "drv8833.h"

DRV8833::DRV8833(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4) : _in1(in1), _in2(in2), _in3(in3), _in4(in4) {
    
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);

    analogWriteResolution(8);
    analogWriteFrequency(20000);

}

void DRV8833::Drive(int16_t left, int16_t right) {

    left = constrain(left, -255, 255);
    if (left > 0) {
        analogWrite(_in1, 255 - left);
        analogWrite(_in2, 255);
    } else if (left < 0) {
        analogWrite(_in1, 255);
        analogWrite(_in2, 255 + left);
    } else {
        analogWrite(_in1, 255);
        analogWrite(_in2, 255);
    }

    right = constrain(right, -255, 255);
    if (right > 0) {
        analogWrite(_in3, 255);
        analogWrite(_in4, 255 - right);
    } else if (right < 0) {
        analogWrite(_in3, 255 + right);
        analogWrite(_in4, 255);
    } else {
        analogWrite(_in3, 255);
        analogWrite(_in4, 255);
    }
}

void DRV8833::StopBoth() {
    analogWrite(_in1, 255);
    analogWrite(_in2, 255);
    analogWrite(_in3, 255);
    analogWrite(_in4, 255);
}

