#ifndef CBSC_MEASUREMENT_STATISTIC_H
#define CBSC_MEASUREMENT_STATISTIC_H

#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include "transfer.h"
#include "spectre.h"
#include "util.h"


void run_bits_and_stride_test(const char *filename);

void run_training_length_test(const char *filename);

#endif