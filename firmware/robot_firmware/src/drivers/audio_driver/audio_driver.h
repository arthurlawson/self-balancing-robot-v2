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

 class AudioDriver {
    public:
        AudioDriver(uint8_t audioPin);
        bool Begin();
        void Update();
        void PlayRandomAudio();
    private:
        void StartPlayback(const char *filepath);
        void StopPlayback();
        uint8_t CountAudioFiles();

        uint8_t _audioPin;
        File _audioFile;
        bool _isPlaying;
        uint8_t _totalAudioFiles;

        unsigned long _lastPlayTime;
        unsigned long _playbackStartTime;
        unsigned long _lastSampleTimeMicros;

        static constexpr uint32_t SAMPLE_DELAY_MICROS = 91; // 11025 Hz Sample Rate
        static constexpr uint32_t MAX_PLAY_DURATION_MILLIS = 3000; // Max playtime of 3 seconds
        static constexpr uint32_t MIN_BETWEEN_PLAYS_MILLIS = 5000; // Minimum time between plays of 5 seconds
 };