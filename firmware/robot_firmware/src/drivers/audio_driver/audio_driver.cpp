#include "audio_driver.h"
#include "driver/gpio.h"

void AudioPlaybackTask(void *pvParameters) {
    AudioDriver *driver = (AudioDriver *)pvParameters;
    
    Serial.printf("[CORE 0] Spawning background audio stream for: %s\n", driver->_currentTrackPath);
    
    File audioFile = LittleFS.open(driver->_currentTrackPath, "r");
    if (!audioFile) {
        Serial.println("[CORE 0] ERROR: Failed to open audio file!");
        driver->_isPlaying = false;
        vTaskDelete(NULL);
        return;
    }

    if (audioFile.available()) {
        audioFile.seek(44, SeekSet); 
    }

    unsigned long startTime = millis();

    static constexpr size_t BUFFER_SIZE = 512;
    uint8_t ramBuffer[BUFFER_SIZE];

    while (audioFile.available() && (millis() - startTime < driver->MAX_PLAY_DURATION_MILLIS) && driver->_isPlaying) {
        size_t bytesRead = audioFile.read(ramBuffer, BUFFER_SIZE);

        for (size_t i = 0; i < bytesRead; i++) {
            if (!driver->_isPlaying) break;

            // Hardware Protection Scaling Layer (Safe, moderate volume peak cap)
            uint8_t scaled_sample = (ramBuffer[i] * 180) / 255;

            ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH, scaled_sample);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH);
            
            delayMicroseconds(driver->SAMPLE_DELAY_MICROS); 
        }
    }

    audioFile.close();
    driver->SmoothTurnOff();

    Serial.println("[CORE 0] Audio stream complete. Terminating thread.");
    
    driver->_isPlaying = false;
    driver->_lastPlayTime = millis();
    vTaskDelete(NULL); 
}

AudioDriver::AudioDriver(uint8_t audioPin)
    : _audioPin(audioPin), _isPlaying(false), _lastPlayTime(0), _totalAudioFiles(0) {

    memset(_currentTrackPath, 0, sizeof(_currentTrackPath));

    }

bool AudioDriver::Begin() {
    if (!LittleFS.begin(true)) return false;

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

    SmoothTurnOff();

    Serial.println("AudioDriver initialized successfully.");
    return true;
}

uint8_t AudioDriver::CountAudioFiles() {
    uint8_t count = 0;

    File root = LittleFS.open("/");
    if (!root || !root.isDirectory()) return 0;

    File file = root.openNextFile();
    while (file) {
        String filename = file.name();
        // Only count if the prefix is "audio" and the file is not a directory
        if (!file.isDirectory() && filename.indexOf("audio") != -1) {
            count++;
        }
        file = root.openNextFile();
    }

    root.close();
    return count;
}

void AudioDriver::PlayRandomAudio() {
    // Dont play if has already played something in the last 5 seconds / is still currently playing something
    if (millis() - _lastPlayTime < AudioDriver::MIN_BETWEEN_PLAYS_MILLIS || _isPlaying || _totalAudioFiles == 0) return;

    int randomFileIndex = random(0, _totalAudioFiles);
    snprintf(_currentTrackPath, sizeof(_currentTrackPath), "/audio%d.wav", randomFileIndex);

    _isPlaying = true;

    xTaskCreatePinnedToCore(
        AudioPlaybackTask,
        "AudioPlaybackTask",
        4096,
        this,
        1,
        NULL,
        0
    );
}

void AudioDriver::StopPlayback() {
    if (!_isPlaying) return;

    _isPlaying = false;
    _lastPlayTime = millis();

    Serial.println("[AUDIO] Stop requested.");
}

void AudioDriver::SmoothTurnOff() {
    uint32_t curDuty = ledc_get_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH);

    while (curDuty > 0) {
        curDuty = (curDuty > 15) ? (curDuty - 15) : 0;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH, curDuty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH);
        delayMicroseconds(150);
    }

    ledc_set_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SPEAKER_CH);

    Serial.println("[AUDIO] Speaker safely turned off and locked low");
}