#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/transfer.h"
#include "../include/spectre.h"

int main() {
    uint8_t *secret = "abcdefghijklmnopqrstuvwxyz";
    uint8_t transfered[27];
    memset(transfered, '\0', 27);

    uint32_t average_not_cached = get_average();
    uint32_t average_cached = get_average_cached();
    printf("Average not cached: %ld\n", average_not_cached);
    printf("Average cached: %ld\n", average_cached);
    size_t threshold = average_not_cached - average_cached;
    printf("Using empirical threshold: \"%d\"\n", threshold);
    printf("Trying to transfer: \"%s\"\n", secret);
    transfer(leak_data, secret, 26, transfered, 27, threshold);
    printf("Result of transfer: \"%s\"\n", transfered);
}