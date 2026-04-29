#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/statistic.h"
#include "../include/util.h"

void help()
{
    printf("usage: \n\
measure <mode> <nr0> <nr1> <nr2> <nr3>\n\
    <mode>\n\
        the covert channel to use options:\n\
            - \"fr\"\n\
                Flush+Reload\n\
            - \"lf\"\n\
                Load+Flush\n\
    <nr0>\n\
        random number generator seed\n\
    <nr1>\n\
        number of bits to transfer\n\
    <nr2>\n\
        stride in covert channel\n\
    <nr3>\n\
        number of predictor training runs\n\
");
    exit(1);
}

int main(int argc, char * argv[])
{
    if (argc < 6)
    {
        help();
    }

    /* Covert Channel Value:
        0 --> Flush+Reload
        1 --> Load+Flush
    */
    uint8_t covert_channel = 0;
    if (!strcmp(argv[1], "fr"))
    {
        covert_channel = 0;
    } else if (!strcmp(argv[1], "lf"))
    {
        covert_channel = 1;
    }
    else {
        help();
    }

    uint32_t random_seed = atoi(argv[2]);
    srand(random_seed);
    uint8_t bit_count = atoi(argv[3]);
    uint64_t stride = atoi(argv[4]);
    uint64_t train_count = atoi(argv[5]);

    measure_ones(bit_count, stride, train_count, covert_channel);

    //run_bits_and_stride_test("bits_and_stride.csv");
    //run_training_length_test("training_length.csv");
}