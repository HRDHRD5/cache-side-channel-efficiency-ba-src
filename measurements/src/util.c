#include "../include/util.h"

void uint_to_bin_str(const uint64_t in, char *out, const uint64_t out_len)
{
    for (uint64_t i = DOUBLE_TIMES(1ul, out_len-1); i > 0; i >>= 1)
    {
        strcat(out, ((in & i) == i) ? "1" : "0");

        if (i == 1)
            i = 0;
    }
}

uint64_t random_uint_64()
{
    uint64_t result = 0;
    uint64_t tmp = 0;
    uint8_t curr;

    for (int i = 0; i < 8; i++)
    {
        curr = rand();
        tmp = curr;
        result += DOUBLE_TIMES(tmp, 8 * i);
    }

    return result;
}