#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/statistic.h"
#include "../include/util.h"


int main()
{
    run_bits_and_stride_test("bits_and_stride.csv");
    run_training_length_test("training_length.csv");
}