#ifndef CBSC_MEASUREMENT_TRANSFER_H
#define CBSC_MEASUREMENT_TRANSFER_H

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "../include/instructions.h"
#include "../include/spectre.h"
#include "../include/constants.h"

size_t get_threshold(uint8_t covert_channel);

bool is_cached_load(uint8_t *address, size_t threshold);

uint64_t transfer_bitwise(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                             uint8_t bits_count, uint64_t stride),
                          uint64_t (*decode)(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold),
                          uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length,
                          uint8_t covert_channel);

uint64_t transfer_array_index(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                                 uint8_t bits_count, uint64_t stride),
                              uint64_t (*decode)(uint8_t side_channel[], uint8_t bits_count, uint64_t stride, size_t threshold),
                              uint64_t in, size_t threshold, uint8_t bits_count, uint64_t stride, uint64_t train_data_length,
                              uint8_t covert_channel);

uint64_t transfer_mixed(void (*asm_encode)(uint64_t data, uint8_t *channel,
                                           uint8_t n_b, uint8_t n_a, uint64_t bitmask, uint64_t stride),
                        uint64_t (*decode)(uint8_t side_channel[], uint8_t n_b, uint8_t n_a, uint64_t stride, size_t threshold),
                        uint64_t in, uint8_t channel[], size_t threshold, uint8_t n_b, uint8_t n_a, uint64_t stride, uint64_t train_data_length,
                        uint8_t covert_channel);

#endif