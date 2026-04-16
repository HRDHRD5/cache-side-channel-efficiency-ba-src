#include "../include/spectre.h"

// returns true during the training phase, false if encoding can only happen transiently
bool leak_data(uint32_t iteration, uint32_t *train_data,
    uint32_t train_data_len, uint64_t secret, uint8_t channel[],
    uint8_t bits_count, uint64_t stride)
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode_data(secret, channel, bits_count, stride);
        return true;
    }
    return false;
} 