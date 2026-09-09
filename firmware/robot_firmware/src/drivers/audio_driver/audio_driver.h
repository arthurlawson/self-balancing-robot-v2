/**
 * @file audio_driver.h
 * @brief Audio driver class for ESP32-S3 microcontroller, providing functionality to play files stored in the data partition
 * controlled using LEDC - LED Control (originally designed for precise PWM control of LEDs), and adjusted for use with 8-bit WAV files. 
 * The driver supports playing random audio files from the LittleFS file system, with a maximum playtime of 3 seconds and a minimum time between 
 * plays of 5 seconds to prevent overheating of the weak 1/4W resistors used.
 * 
 * @author Arthur Lawson
 */

#pragma once

#include <Arduino.h>
#include "LittleFS.h"
#include "driver/ledc.h"

#define SPEAKER_CH     LEDC_CHANNEL_0
#define SPEAKER_TIMER  LEDC_TIMER_0

void AudioPlaybackTask(void *pvParameters);

class AudioDriver {
    friend void AudioPlaybackTask(void *pvParameters);

    public:
        AudioDriver(uint8_t audioPin);
        bool Begin();
        void PlayRandomAudio();
        void StopPlayback();    

    private:
        uint8_t _audioPin;
        uint8_t _totalAudioFiles;

        char _currentTrackPath[32];
        volatile bool _isPlaying;
        unsigned long _lastPlayTime;
        
        static constexpr uint32_t MIN_BETWEEN_PLAYS_MILLIS = 5000; // Minimum time between plays of 5 secondsv
        static constexpr uint32_t MAX_PLAY_DURATION_MILLIS = 3000; // Max playtime of 3 seconds
        static constexpr uint32_t SAMPLE_DELAY_MICROS = 91;        // 11025 Hz Sample Rate

        uint8_t CountAudioFiles();
        void SmoothTurnOff();
        
};