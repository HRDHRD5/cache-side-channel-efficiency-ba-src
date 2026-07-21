#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "./include/statistic.h"
#include "./include/util.h"

void help()
{
    printf("usage: \n\
measure <mode> <nr0> <nr1> <nr2> <nr3>\n\
    <mode>\n\
        the covert channel to use options:\n\
            - \"frb\"\n\
                Flush+Reload Bitwise\n\
            - \"lfb\"\n\
                Load+Flush Bitwise\n\
            - \"fra\"\n\
                Flush+Reload Array Index\n\
            - \"frm\"\n\
                Flush+Reload mixed Encoding\n\
            - \"mprop\"\n\
                Measure mixed Encoding Properties\n\
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
        0 --> Flush+Reload Bitwise
        1 --> Load+Flush Bitwise
        2 --> Flush+Reload Array Index
        42 --> Flush+Reload Encoding mixed
    */
    uint8_t covert_channel = 0;
    if (!strcmp(argv[1], "frb"))
    {
        covert_channel = 0;
    } else if (!strcmp(argv[1], "frm") || !strcmp(argv[1], "mprop"))
    {
        covert_channel = 42;
        if (argc < 7)
        {
            help();
        }
    } else if (!strcmp(argv[1], "lfb"))
    {
        covert_channel = 1;
    } else if (!strcmp(argv[1], "fra"))
    {
        covert_channel = 2;
    }
    else {
        help();
    }

    uint32_t random_seed = atoi(argv[2]);
    srand(random_seed);
    uint8_t n_b = atoi(argv[3]);
    uint8_t n_a = atoi(argv[3]);
    uint64_t stride = atoi(argv[4]);
    uint64_t train_count = atoi(argv[5]);
    if (covert_channel == 42)
    {
        n_a = atoi(argv[4]);
        stride = atoi(argv[5]);
        train_count = atoi(argv[6]);
        if (!strcmp(argv[1], "frm"))
        {
            measure_mixed(n_b, n_a, stride, train_count, covert_channel);
        }
        else
        {
            measure_mixed_properties(n_b, n_a, stride, train_count, covert_channel);
        }
    }
    else
    {
        measure_ones(n_b, stride, train_count, covert_channel);
    }
}