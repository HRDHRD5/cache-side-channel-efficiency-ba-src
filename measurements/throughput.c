#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "./include/statistic.h"
#include "./include/util.h"

uint64_t stride = 4096;
uint32_t training = 20;
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
    bool train = false;

    uint32_t train_data[((training) * stride) + 1];
    for (uint32_t i = 0; i < (training * stride) + 1; i += stride)
    {
        train_data[i] = i;
    }

    // empty the output
    size_t end = 0;
    size_t start = rdtsc();

    memset(side_channel, 5, channel_array_size);

    clflush(&train_data[(training*stride)]);

    // flushing the transfer array, before encoding data into it
    for (uint32_t i = stride; i < channel_array_size; i += stride)
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
            //train = leak_data_dynamic(asm_encode_load_dynamic, i, train_data, training * stride, 0, side_channel+stride, n_b, n_a, bitmask, stride);
            train = leak_data(asm_encode_load_bitwise, i, train_data, training * stride, 0, side_channel+stride, n_b, stride);
            //printf("Training: %d\n", train);
        }
    }

    // writing secret into the side channel array
    //train = leak_data_dynamic(asm_encode_load_dynamic, (training*stride), train_data, training * stride, in, side_channel+stride, n_b, n_a, bitmask, stride);
    train = leak_data(asm_encode_load_bitwise, (training*stride), train_data, training * stride, in, side_channel+stride, n_b, stride);
    transfer_result = decode_flush_reload_dynamic(side_channel+stride, n_b, n_a, stride, threshold);
    //transfer_result = decode_flush_reload_bitwise(side_channel+stride, n_b, stride, threshold);

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
    leak_data_dynamic(asm_encode_load_dynamic, 0, &train, 1, 0, side_channel+stride, n_b, n_a, bitmask, stride);

    return rdtsc() - start;
}

int main(int argc, char * argv[])
{
    if (argc < 5)
    {
        help();
    }

    n_b = atoi(argv[2]);
    n_a = atoi(argv[3]);
    uint32_t random_seed = atoi(argv[4]);
    srand(random_seed);
    uint64_t in = random_uint_64();

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
        runtime = measure_transfer_dynamic(in);
    } else if (!strcmp(argv[1], "lfb"))
    {
    } else if (!strcmp(argv[1], "fra"))
    {
    }
    else {
        help();
        return 1;
    }


    char secret_str[65];
    char result_str[65];
    memset(secret_str, '\0', 65);
    memset(result_str, '\0', 65);
    uint_to_bin_str(in, secret_str, n_b * n_a);
    uint_to_bin_str(transfer_result, result_str, n_b * n_a);

    printf("%zu;%zu;%zu;%s;%s;%zu\n",
           n_b * n_a, stride, training, secret_str, result_str, runtime);

    free(side_channel);

    return 0;
}