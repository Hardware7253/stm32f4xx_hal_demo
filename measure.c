#include <stdint.h>
#include "stm32f4xx_hal.h"

// Max test frequency is 100 KHz
// Going beyond this the max sample rate can easily be exceeded (2 Msps max)
#define SAMPLES_PER_PERIOD 16 
#define SAMPLE_PERIODS 5 
#define SAMPLES SAMPLES_PER_PERIOD + SAMPLE_PERIODS

static uint16_t sin_samples[SAMPLES] = {0};
static uint16_t cos_samples[SAMPLES] = {0};
static uint16_t ap_samples[SAMPLES] = {0}; // AP for arbitrary phase (measured afer DUT`)

// Get the sample priod in ns
static uint32_t get_sample_t(uint32_t test_f) {
    return 1000000000ULL / ((uint64_t)test_f * SAMPLES_PER_PERIOD);
}

// Hello me this shit might not be necessry if the adc is triggered by the hal and uses DMA

// Record input and output voltage waveforms for the DUT
// This involves an ADC for the input and output voltage
// Samples are read at a consistent rate to match defines at the top of the document
// The ADCs are read at seperate known times because simultaneous reading is not possible

// Plan will be to use tim, dma, and two adcs
// IDK how to activate both tims synchronously
void record_samples(uint32_t test_f) {
    // 1. Setup test waveform
    // 2. Recording sin and ap samples:

    // Recording from two ADCs simultaneously will not be
    // possible without changing the phase relationship at high frequencies
    // If both buffers were able 
    // So the two buffers will need to read

    //

    // 2. record sin and ap samples
    // 3. 

}