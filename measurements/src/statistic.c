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

    size_t start = rdtsc();

    // adding one cache line padding to stride based on empirical testing
    if (covert_channel == 0)
    {
        transfered = transfer_bitwise(asm_encode_load_bitwise, decode_flush_reload_bitwise, secret, threshold, bits_count, stride + 64, training_data_length, covert_channel);
    }
    else if (covert_channel == 1)
    {
        transfered = transfer_bitwise(asm_encode_flush_bitwise, decode_load_flush_bitwise, secret, threshold, bits_count, stride + 64, training_data_length, covert_channel);
    }
    else if (covert_channel == 2)
    {
        transfered = transfer_array_index(asm_encode_load_array_index, decode_flush_reload_array_index, secret, threshold, bits_count, stride + 64, training_data_length, covert_channel);
    }

    size_t timediff = rdtsc() - start;

    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    uint_to_bin_str(secret, secret_str, bits_count);
    uint_to_bin_str(transfered, result_str, bits_count);

    printf("%zu;%zu;%zu;%s;%s;%zu\n",
           bits_count, stride, training_data_length, secret_str, result_str, timediff);
}

void measure_mixed(uint8_t n_b, uint8_t n_a, uint64_t stride, uint64_t training_data_length, uint8_t covert_channel)
{
    lfence();
    mfence();
    // nanosleep(&req, NULL);

    // setting random secret
    uint64_t secret = random_uint_64();
    uint64_t transfered = 0;

    // getting a new threshold for every run
    size_t threshold = get_threshold(covert_channel);

    size_t channel_array_size = (size_t)(((n_b * (powl(2, n_a) - 1)) + 3) * stride);
    uint8_t *side_channel = (uint8_t*) malloc(channel_array_size);
    //printf("Channel address: %zu\n", side_channel);
    memset(side_channel, 5, channel_array_size);

    size_t start = rdtsc();

    // adding one cache line padding to stride based on empirical testing
    if (covert_channel == 42)
    {
        transfered = transfer_mixed(asm_encode_load_dynamic, decode_flush_reload_dynamic, secret, side_channel, threshold, n_b, n_a, stride + 64, training_data_length, covert_channel);
    }
    else
    {
        printf("Channel unsupported.");
        exit(1);
    }

    size_t timediff = rdtsc() - start;

    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    uint_to_bin_str(secret, secret_str, n_b*n_a);
    uint_to_bin_str(transfered, result_str, n_b*n_a);

    printf("%zu;%zu;%zu;%zu;%s;%s;%zu\n",
           n_b, n_a, stride, training_data_length, secret_str, result_str, timediff);
}

void measure_mixed_properties(uint8_t n_b, uint8_t n_a, uint64_t stride, uint64_t training_data_length, uint8_t covert_channel)
{
    lfence();
    mfence();
    // nanosleep(&req, NULL);

    // setting random secret
    uint64_t secret = random_uint_64();
    uint64_t transfered = 0;

    // getting a new threshold for every run
    size_t threshold = get_threshold(covert_channel);

    size_t channel_array_size = (size_t)(((n_b * (powl(2, n_a) - 1)) + 3) * stride);
    uint8_t *side_channel = (uint8_t*) malloc(channel_array_size);
    //printf("Channel address: %zu\n", side_channel);
    memset(side_channel, 5, channel_array_size);

    size_t start = rdtsc();

    // adding one cache line padding to stride based on empirical testing
    if (covert_channel == 42)
    {
        transfered = transfer_mixed_properties(asm_encode_load_dynamic, decode_flush_reload_dynamic_print_props, secret, side_channel, threshold, n_b, n_a, stride + 64, training_data_length, covert_channel);
    }
    else
    {
        printf("Channel unsupported.");
        exit(1);
    }

    size_t timediff = rdtsc() - start;

    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    uint_to_bin_str(secret, secret_str, n_b*n_a);
    uint_to_bin_str(transfered, result_str, n_b*n_a);

    printf("%zu;%zu;%zu;%zu;%s;%s;%zu\n",
           n_b, n_a, stride, training_data_length, secret_str, result_str, timediff);
}
