#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "../include/instructions.h"
#include "../include/spectre.h"
#include "../include/constants.h"


#ifndef CBSC_MEASUREMENT_TRANSFER_H
#define CBSC_MEASUREMENT_TRANSFER_H

uint32_t get_average();
uint32_t get_average_cached();

bool is_cached(uint8_t *address, size_t threshold);

uint64_t transfer(bool (*leak_data)(uint32_t iteration, uint32_t *train_data, uint32_t train_data_len,
                        uint64_t secret, uint8_t channel[],
                        uint8_t bits_count, uint64_t stride),
         uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length);

#endif