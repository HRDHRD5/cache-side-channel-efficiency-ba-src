#include "../include/statistic.h"

void run_transfer_statistics_tests(const char *filename)
{
    FILE *out = fopen(filename, "w");
    fprintf(out, "Index;Transfer Bit Count;Input Binary;Output Binary\n");


    uint8_t *secret = "d"; //bcdefghijklmnopqrstuvwxyz";
    uint8_t transfered[27];
    memset(transfered, '\0', 27);

    uint32_t average_not_cached = get_average();
    uint32_t average_cached = get_average_cached();
    printf("Average not cached: %ld\n", average_not_cached);
    printf("Average cached: %ld\n", average_cached);
    size_t threshold = average_not_cached - average_cached;
    printf("Using empirical threshold: \"%d\"\n", threshold);
    printf("Trying to transfer: \"%s\"\n", secret);
    transfer(leak_data, secret, 1, transfered, 27, threshold);
    printf("Result of transfer: \"%s\"\n", transfered);

    fclose(out);
}