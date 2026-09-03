#include "audio_driver.h"
#include "rom/gpio.h"
#include "driver/gpio.h"

AudioDriver::AudioDriver(uint8_t audioPin)
    : _audioPin(audioPin), _isPlaying(false), _lastPlayTime(0), _playbackStartTime(0), _lastSampleTimeMicros(0) {

        gpio_reset_pin((gpio_num_t)_audioPin);
        pinMode(_audioPin, OUTPUT);
        digitalWrite(_audioPin, LOW);
    }

bool AudioDriver::Begin() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Begin Failed.");
        return false;
    }

    _totalAudioFiles = CountAudioFiles();
    Serial.printf("Total audio files found: %d\n", _totalAudioFiles);

    // 100kHz carrier frequency to prevent high pitch speaker noise.
    // 8-bit resolution for 8-bit WAV files.
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = SPEAKER_TIMER,
        .freq_hz = 100000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = _audioPin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SPEAKER_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = SPEAKER_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    StopPlayback();

    Serial.println("AudioDriver initialized successfully.");
    return true;
}

uint8_t AudioDriver::CountAudioFiles() {
    uint8_t count = 0;

    File root = LittleFS.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open the root directory for counting");
        return 0;
    }

    File file = root.openNextFile();
    while (file) {
        // Only count if the prefix is "audio" and the file is not a directory
        if (!file.isDirectory() && strstr(file.name(), "audio") != nullptr) {
            count++;
        }
        file = root.openNextFile();
    }

    root.close();
    return count;
}

void AudioDriver::PlayRandomAudio() {
    if (_totalAudioFiles == 0) {
        Serial.println("AUDIO ERROR: No audio files found to play.");
        return;
    }

    unsigned long now = millis();
    // Safety cooling guard protects the weak 1/4W resistors from overheating.
    if (now - _lastPlayTime < MIN_BETWEEN_PLAYS_MILLIS || _isPlaying) return;

    int randomFileIndex = random(0, _totalAudioFiles);
    char filepath[32];
    snprintf(filepath, sizeof(filepath), "/audio%d.wav", randomFileIndex);
    
    StartPlayback(filepath);
}

void AudioDriver::StartPlayback(const char *filepath) {
    _audioFile = LittleFS.open(filepath, "r");
    if (!_audioFile) {
        Serial.printf("Failed to open audio file: %s\n", filepath);
        return;
    }

    // Skip the 44-byte WAV Header
    if (_audioFile.available()) {
        _audioFile.seek(44, SeekSet);
    }

    // Re-bind the pin to the LEDC hardware timer only when audio needs to actively play out the speaker
    ledc_channel_config_t ledc_channel = {
        .gpio_num = _audioPin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = SPEAKER_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = SPEAKER_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);

    _isPlaying = true;
    _playbackStartTime = millis();
    _lastSampleTimeMicros = micros();
    Serial.printf("AUDIO playing: %s\n", filepath);
}

void AudioDriver::Update() {
    if (!_isPlaying) return;

    unsigned long now = millis();
    if (now - _playbackStartTime >= MAX_PLAY_DURATION_MILLIS) {
        StopPlayback();
        return;
    }

    uint32_t nowMicros = micros();
    if (nowMicros - _lastSampleTimeMicros >= SAMPLE_DELAY_MICROS) {
        uint8_t wavSample = _audioFile.read();

        // Feed the sample to the LEDC channel for playback. 
        // The LEDC peripheral auto handles the PWM output based on the set duty cycle.
        ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH, wavSample);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH);

        _lastSampleTimeMicros = nowMicros;
    }
}

void AudioDriver::StopPlayback() {
    _isPlaying = false;
    if (_audioFile) {
        _audioFile.close();
    }

    ledc_stop(LEDC_LOW_SPEED_MODE, SPEAKER_CH, 0);
    gpio_reset_pin((gpio_num_t)_audioPin);
    pinMode(_audioPin, OUTPUT);
    digitalWrite(_audioPin, LOW);

    _lastPlayTime = millis();
    Serial.println("AUDIO stopped.");
}