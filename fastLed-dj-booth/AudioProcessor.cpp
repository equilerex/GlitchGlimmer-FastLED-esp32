#include "AudioProcessor.h"
#include "Config.h"

AudioProcessor::AudioProcessor(int wsPin, int sdPin, int sckPin)
    : _wsPin(wsPin), _sdPin(sdPin), _sckPin(sckPin),
      previousVolume(0), lastBeatTime(0), currentBPM(0.0f) {
    try {
        FFT = new ArduinoFFT<double>(vReal, vImag, NUM_SAMPLES, SAMPLE_RATE);
    } catch (...) {
        FFT = nullptr;
        // Handle error
    }
}


void AudioProcessor::begin() {
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = _sckPin,
        .ws_io_num = _wsPin,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = _sdPin
    };

    esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) return;

    err = i2s_set_pin(I2S_PORT, &pin_config);
    if (err != ESP_OK) {
        i2s_driver_uninstall(I2S_PORT);
        return;
    }

    i2s_start(I2S_PORT);
}

void AudioProcessor::captureAudio() {
    size_t bytesRead;
    int32_t raw32[NUM_SAMPLES];
    i2s_read(I2S_PORT, &raw32, sizeof(raw32), &bytesRead, portMAX_DELAY);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        buffer[i] = raw32[i] >> 16; // Convert to int16_t for display
        vReal[i] = (double)raw32[i] / 2147483648.0; // Normalize to [-1.0, 1.0]
        vImag[i] = 0.0;
    }
}

AudioFeatures AudioProcessor::analyzeAudio() {
    AudioFeatures features = {0};

    // Volume = RMS
    double sumSquares = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sumSquares += vReal[i] * vReal[i];
    }
    features.volume = sqrt(sumSquares / NUM_SAMPLES);

    // Beat detection
    double volumeChange = features.volume - previousVolume;
    unsigned long currentTime = millis();
    static unsigned long lastBeatInterval = 0;
    if (volumeChange > 0.05 && (currentTime - lastBeatTime) > 300) {
        features.beatDetected = true;
        lastBeatInterval = currentTime - lastBeatTime;
        if (lastBeatInterval > 250 && lastBeatInterval < 2000) {
            currentBPM = 60000.0 / lastBeatInterval;
        }
        lastBeatTime = currentTime;
    } else {
        features.beatDetected = false;
    }
    previousVolume = features.volume;

    // FFT
    FFT->windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT->compute(FFT_FORWARD);
    FFT->complexToMagnitude();

    // Copy spectrum
    for (int i = 0; i < NUM_SAMPLES / 2; i++) {
        features.spectrum[i] = vReal[i];
    }

    // Frequency bands
    int bassEnd = (int)(200.0 * NUM_SAMPLES / SAMPLE_RATE);
    int midEnd = (int)(2000.0 * NUM_SAMPLES / SAMPLE_RATE);
    int trebleEnd = NUM_SAMPLES / 2;

    double bassSum = 0.0, midSum = 0.0, trebleSum = 0.0;
    for (int i = 0; i < bassEnd; i++) bassSum += vReal[i];
    for (int i = bassEnd; i < midEnd; i++) midSum += vReal[i];
    for (int i = midEnd; i < trebleEnd; i++) trebleSum += vReal[i];
static float smoothedDb = -60;
smoothedDb = 0.8 * smoothedDb + 0.2 * features.decibels;
features.decibels = smoothedDb;
    features.bass = bassSum / bassEnd;
    features.mid = midSum / (midEnd - bassEnd);
    features.treble = trebleSum / (trebleEnd - midEnd);


    return features;
}

const double* AudioProcessor::getFFTData() const {
    return vReal;
}

const int16_t* AudioProcessor::getRawAudio() const {
    return buffer;
}

float AudioProcessor::getCurrentBPM() const {
    return currentBPM;
}

AudioProcessor::~AudioProcessor() {
    delete FFT;
}
