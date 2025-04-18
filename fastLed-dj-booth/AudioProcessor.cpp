#include "AudioProcessor.h"
#include "Config.h"

AudioProcessor::AudioProcessor()
    : previousVolume(0.0), lastBeatTime(0), currentBPM(0.0),
      normalizedVolume(0.0), rollingMin(1.0), rollingMax(0.0),
      FFT(nullptr)
{
    FFT = new ArduinoFFT<double>(vReal, vImag, NUM_SAMPLES, SAMPLE_RATE);
}

AudioProcessor::~AudioProcessor() {
    delete FFT;
}

void AudioProcessor::begin() {
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    i2s_start(I2S_PORT);
}

void AudioProcessor::captureAudio() {
    size_t bytesRead = 0;
    int32_t i2sBuffer[NUM_SAMPLES];
    i2s_read(I2S_PORT, (void*)i2sBuffer, sizeof(i2sBuffer), &bytesRead, portMAX_DELAY);
    int samplesRead = bytesRead / sizeof(int32_t);

    for (int i = 0; i < samplesRead && i < NUM_SAMPLES; i++) {
        float normalized = i2sBuffer[i] / 8388608.0f;  // Normalize 24-bit signed PCM
        vReal[i] = normalized;
        vImag[i] = 0.0;
        buffer[i] = (int16_t)(normalized * 32767);  // For waveform
    }

    // Fallback fill if not enough samples
    for (int i = samplesRead; i < NUM_SAMPLES; i++) {
        vReal[i] = 0.0;
        vImag[i] = 0.0;
        buffer[i] = 0;
    }
}

AudioFeatures AudioProcessor::analyzeAudio() {
    AudioFeatures features = {};

    // Calculate RMS volume
    double sumSquares = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        vReal[i] = constrain(vReal[i], -1.0f, 1.0f);  // Clamp inputs
        sumSquares += vReal[i] * vReal[i];
    }

    double rawVolume = sqrt(sumSquares / NUM_SAMPLES);
    normalizedVolume = gainSmoothing * normalizedVolume + (1 - gainSmoothing) * rawVolume;
    features.volume = normalizedVolume;

    // loudness (with smoothing)
    // Loudness mapped to a smoother 0–100 scale
    static float smoothedLoudness = 0;
    float rawLoudness = features.volume * 100.0;  // scale RMS to 0–100
    smoothedLoudness = 0.9 * smoothedLoudness + 0.1 * rawLoudness;
    features.loudness = constrain(smoothedLoudness, 0, 100);

    //Serial.printf("Raw Volume: %.2f | Smoothed: %.2f | dB: %.2f\n", rawVolume, normalizedVolume, smoothedLoud);

    // Beat detection
    double volumeChange = features.volume - previousVolume;
    unsigned long now = millis();
    if (volumeChange > 0.05 && (now - lastBeatTime) > 250) {
        features.beatDetected = true;
        unsigned long beatInterval = now - lastBeatTime;
        if (beatInterval > 250 && beatInterval < 2000) {
            currentBPM = 60000.0 / beatInterval;
        }
        lastBeatTime = now;
    } else {
        features.beatDetected = false;
    }
    features.bpm = currentBPM;

    // FFT
    if (FFT) {
        FFT->windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT->compute(FFT_FORWARD);
        FFT->complexToMagnitude();
    }

    // Preview first few spectrum bins
    String spectrumLog = "Spectrum[0-10]: ";
    for (int i = 0; i < 10; i++) {
        spectrumLog += String(vReal[i], 2) + " ";
    }
    //Serial.println(spectrumLog);

    // ---- Frequency bands ----
    //Serial.println(">> Entering frequency band calculation...");

    // Copy spectrum
    for (int i = 0; i < NUM_SAMPLES / 2; i++) {
        features.spectrum[i] = vReal[i];
    }

    int bassLimit = (int)(200.0 * NUM_SAMPLES / SAMPLE_RATE);
    int midLimit = (int)(2000.0 * NUM_SAMPLES / SAMPLE_RATE);

    double bassSum = 0.0, midSum = 0.0, trebleSum = 0.0;

    for (int i = 0; i < bassLimit; i++) {
        bassSum += vReal[i];
    }

    for (int i = bassLimit; i < midLimit; i++) {
        midSum += vReal[i];
    }

    for (int i = midLimit; i < NUM_SAMPLES / 2; i++) {
        trebleSum += vReal[i];
    }

    features.bass   = bassLimit > 0 ? bassSum / bassLimit : 0.0;
    features.mid    = (midLimit - bassLimit) > 0 ? midSum / (midLimit - bassLimit) : 0.0;
    features.treble = (NUM_SAMPLES / 2 - midLimit) > 0 ? trebleSum / (NUM_SAMPLES / 2 - midLimit) : 0.0;

    #if AUDIO_DEBUG
        Serial.printf("Bands | Bass: %.2f | Mid: %.2f | Treble: %.2f\n",
                    features.bass, features.mid, features.treble);

    #endif

    previousVolume = features.volume;
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

float AudioProcessor::getNormalizedVolume() const {
    return normalizedVolume;
}