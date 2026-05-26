#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "./include/statistic.h"
#include "./include/util.h"

uint64_t stride = 4096;
uint32_t training = 50;
uint64_t threshold;
uint64_t n_a;
uint64_t n_b;
uint64_t transfer_result = 0;
uint64_t bitmask;
uint8_t *side_channel;
size_t channel_array_size;

void help()
{
    printf("usage: \n\
throughput <function> <nr0> <nr1> <nr2>\n\
    <mode>\n\
        the covert channel to use options:\n\
            - \"frb\"\n\
                Flush+Reload Bitwise\n\
            - \"lfb\"\n\
                Load+Flush Bitwise\n\
            - \"fra\"\n\
                Flush+Reload Array Index\n\
    <nr0>\n\
        n_b\n\
    <nr1>\n\
        n_a\n\
    <nr2>\n\
        n_b\n\
");
    exit(1);
}

size_t measure_transfer_dynamic(uint64_t in)
{
    bool train = 0;

    uint32_t train_data[(training * stride)+1];
    for (uint32_t i = 0; i < (training * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    size_t end = 0;
    size_t start = rdtsc();

    clflush(&train_data[(training*stride)]);

    // flushing the transfer array, before encoding data into it
    for (uint32_t i = 0; i < channel_array_size; i += stride)
        clflush(&side_channel[i]);

    maccess(&in);

    lfence();
    mfence();

    // training the predictor
    if (training > 0)
    {
        for (uint32_t i = 0; i < (training * stride); i += stride)
        {
            // using 0 as secret, so no adresses are loaded into cache
            leak_data_dynamic(i, train_data, training, 0, side_channel+stride, n_b, n_a, bitmask, stride);
            // printf("Training: %d\n", training);
        }
    }

    // writing secret into the side channel array
    train = leak_data_dynamic((training*stride), train_data, training, in, side_channel+stride, n_b, n_a, bitmask, stride);
    transfer_result = decode_flush_reload_dynamic(side_channel+stride, n_b, n_a, stride, threshold);

    end = rdtsc() - start;
    printf("Training: %d\n", train);
    return end;
}

size_t measure_t_t()
{
    uint32_t train = 0;
    prefetch(&train);
    lfence();
    mfence();

    size_t start = rdtsc();
    // train address can be cached, because thats the case with training runs
    leak_data_dynamic(0, &train, 1, 0, side_channel+stride, n_b, n_a, bitmask, stride);

    return rdtsc() - start;
}

int main(int argc, char * argv[])
{
    if (argc < 4)
    {
        help();
    }

    n_b = atoi(argv[2]);
    n_a = atoi(argv[3]);

    channel_array_size = (size_t)((powl(2, n_a)) * stride * n_b) + 3;
    side_channel = (uint8_t*) malloc(channel_array_size);
    memset(side_channel, 5, channel_array_size);
    bitmask = 1;
    for (int i = 0; i < n_a; ++i)
    {
        bitmask = bitmask << 1;
        bitmask += 1;
    }
    threshold = get_threshold(0);


    size_t runtime = 0;
    if (!strcmp(argv[1], "trnsf"))
    {
        runtime = measure_transfer_dynamic(1);
    } else if (!strcmp(argv[1], "lfb"))
    {
    } else if (!strcmp(argv[1], "fra"))
    {
    }
    else {
        help();
        return 1;
    }
    printf("%s;%zu;%zu\n",
           argv[1], runtime, n_a);

    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    //uint_to_bin_str(secret, secret_str, bits_count);
    uint_to_bin_str(transfer_result, result_str, n_b * n_a);
    printf("Result: %s\n", result_str);

    return 0;
}