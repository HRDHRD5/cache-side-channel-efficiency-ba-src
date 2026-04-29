#include "../include/statistic.h"

const struct timespec req = {
    .tv_nsec = 1000000, // 1ms
};

void measure_ones(uint8_t bits_count, uint64_t stride, uint64_t training_data_length, uint8_t covert_channel)
{
    lfence();
    mfence();
    // nanosleep(&req, NULL);

    // setting random secret
    uint64_t secret = random_uint_64();
    uint64_t transfered = 0;

    // getting a new threshold for every run
    size_t threshold = get_threshold(covert_channel);
    get_threshold(1);

    // adding one cache line padding to stride based on empirical testing
    if (covert_channel == 0)
    {
        transfered = transfer(leak_data_flush_reload, decode_flush_reload, secret, threshold, bits_count, stride + 64, training_data_length, covert_channel);
    }
    else if (covert_channel == 1)
    {
        transfered = transfer(leak_data_load_flush, decode_load_flush, secret, threshold, bits_count, stride + 64, training_data_length, covert_channel);
    }

    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    uint_to_bin_str(secret, secret_str, bits_count);
    uint_to_bin_str(transfered, result_str, bits_count);

    printf("%zu;%zu;%zu;%s;%s\n",
           bits_count, stride, training_data_length, secret_str, result_str);
}

void run_bits_and_stride_test(const char *filename)
{
    FILE *out = fopen(filename, "w");
    fprintf(out, "index;bit_count;stride;input;output\n");

    uint64_t secret;
    uint64_t transfered;
    uint64_t index = 0;

    // Repeating experiment for N Times
    for (uint64_t n = 0; n < NUMBER_OF_STATISTICS_MEASUREMENTS; n++)
        // Trying different transfer lengths from 4 to 32 bit
        for (int bits_count = 4; bits_count < 16; bits_count++)
            // Trying different Strides (min 64 bit --> L1 Cache line size)
            for (int stride = 64; stride <= 4096; stride *= 2)
            {
                lfence();
                mfence();
                // nanosleep(&req, NULL);

                // setting random secret
                secret = random_uint_64();
                transfered = 0;

                // getting a new threshold for every run
                size_t threshold = get_threshold(0);
                // printf("Using empirical threshold: \"%d\"\n", threshold);

                // printf("Trying to transfer: \"%zu\"\n", secret);
                transfered = transfer(leak_data_flush_reload, decode_flush_reload, secret, threshold, bits_count, stride, TRAIN_DATA_LENGTH, 0);
                // printf("Result of transfer: \"%zu\"\n", transfered);

                char secret_str[65];
                char result_str[65];
                memset(secret_str, '\0', 65);
                memset(result_str, '\0', 65);
                uint_to_bin_str(secret, secret_str, bits_count);
                uint_to_bin_str(transfered, result_str, bits_count);

                fprintf(out, "%zu;%zu;%zu;%s;%s\n",
                        index, bits_count, stride, secret_str, result_str);

                index++;
            }

    fclose(out);
}

void run_training_length_test(const char *filename)
{
    FILE *out = fopen(filename, "w");
    fprintf(out, "index;training_length;input;output\n");

    uint64_t secret;
    uint64_t transfered;
    uint64_t index = 0;

    // Trying different training lengths
    for (int training_data_length = 5; training_data_length <= 150; training_data_length += 5)
        // Repeating experiment for N Times
        for (uint64_t n = 0; n < NUMBER_OF_STATISTICS_MEASUREMENTS; n++)
        {
            lfence();
            mfence();
            // nanosleep(&req, NULL);

            // setting random secret
            secret = random_uint_64();
            transfered = 0;

            // getting a new threshold for every run
            size_t threshold = get_threshold(0);

            transfered = transfer(leak_data_flush_reload, decode_flush_reload, secret, threshold, 8, STRIDE, training_data_length, 0);

            char secret_str[65];
            char result_str[65];
            memset(secret_str, '\0', 65);
            memset(result_str, '\0', 65);
            uint_to_bin_str(secret, secret_str, 8);
            uint_to_bin_str(transfered, result_str, 8);

            fprintf(out, "%zu;%zu;%s;%s\n",
                    index, training_data_length, secret_str, result_str);

            index++;
        }

    fclose(out);
}