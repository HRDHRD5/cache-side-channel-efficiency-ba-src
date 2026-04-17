#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/statistic.h"
#include "../include/util.h"


int main(int argc, char * argv[])
{
    if (argc < 5)
    {
        printf("usage: \n\
measure <nr0> <nr1> <nr2> <nr3>\n\
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

    uint32_t random_seed = atoi(argv[1]);
    srand(random_seed);
    uint8_t bit_count = atoi(argv[2]);
    uint64_t stride = atoi(argv[3]);
    uint64_t train_count = atoi(argv[4]);

    measure_ones(bit_count, stride, train_count);

    //run_bits_and_stride_test("bits_and_stride.csv");
    //run_training_length_test("training_length.csv");
}